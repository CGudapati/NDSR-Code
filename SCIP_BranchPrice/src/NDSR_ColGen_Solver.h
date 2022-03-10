//
//  NDSR_Solver.hpp
//  NDSR_basemodel
//
//  Created by Naga V Gudapati on 12/17/19.
//  Copyright © 2019 Naga V Gudapati. All rights reserved.
//

#ifndef NDSR_ColGen_Solver_h
#define NDSR_ColGen_Solver_h


#include "get_RCSPP_for_all_commodities.h"

#include "NDSR_Instance.hpp"
#include "Timer.hpp"
#include "helper.h"
#include "pricer_NDSR.hpp"
#include "RCSPP_graph.hpp"

#include <chrono>
#include <map>
#include <scip/scip.h>
#include <scip/scipdefplugins.h>
#include <sstream>
#include <unordered_set>
#include <vector>

using paths_per_comm = std::vector<std::vector<std::vector<int>>>; // I really hate typing this goddamn thing again and again

int SCIP_solve_NDSR_colgen(NDSR_data &instance, RCSPP::RCSPP_graph &rcspp_graph, bool continuous_flag, bool include_3_or_flag, bool include_1_cut_if_flag, bool include_1_or_if_flag)
{

        const auto list_of_arcs = instance.get_list_of_arcs();
        const int num_commodities = instance.get_num_commodities();
        const int num_nodes = instance.get_num_nodes();
        const int num_arcs = instance.get_num_arcs();
        const int num_metrics = instance.get_num_metrics();
        const auto arc_idx_dict = instance.get_arc_idx_dict();
        const auto arc_metrics_vec = instance.get_arc_metrics_vec();
        const auto comm_weight_info = instance.get_commodity_weights_info();
        const auto comm_s_t_info = instance.get_commodity_s_t_info();
        const auto variable_costs = instance.get_variable_costs_vec();
        const auto arc_metric_weight_info = instance.get_arc_metrics_vec();

        SCIP *scip = nullptr;         // Declaring the scip environment
        SCIP_CALL(SCIPcreate(&scip)); // Creating the SCIP environment

        /* include default plugins */
        SCIP_CALL(SCIPincludeDefaultPlugins(scip));

        /* Creating the SCIP Problem. */

        SCIP_CALL(SCIPcreateProbBasic(scip, "NDSR_ColGen"));

        SCIP_CALL(SCIPsetObjsense(scip, SCIP_OBJSENSE_MINIMIZE));

        std::vector<int> num_paths_added_for_commodity(num_commodities, 0);

        paths_per_comm paths_added_per_commodity; // check line 25 for the type alias

        paths_added_per_commodity.resize(num_commodities);

        std::cout << "Begin Creating Master: "
                  << "\n";

        //    #Let us define the fixed variables for each arc and add them to the
        //    model. We also add their objective coefficient

        std::vector<std::vector<SCIP_VAR *>> z_vars(num_nodes, std::vector<SCIP_VAR *>(num_nodes)); // This will hold pointers to z variables

        std::ostringstream namebuf;

        auto fixed_costs = instance.get_fixed_costs();

        auto z_var_type = SCIP_VARTYPE_BINARY;

        if (continuous_flag)
        {
                z_var_type = SCIP_VARTYPE_CONTINUOUS;
        }

        for (auto &elem : instance.get_list_of_arcs())
        {
                SCIP_VAR *var = nullptr;
                namebuf.str("");
                namebuf << "z[" << elem.first << "," << elem.second << "]";

                SCIP_CALL(SCIPcreateVar(
                    scip, &var, namebuf.str().c_str(), 0.0, 1.0, fixed_costs[elem.first][elem.second], z_var_type,
                    true,  // initial
                    false, // forget the rest ...
                    nullptr, nullptr, nullptr, nullptr, nullptr));

                // SCIP_CALL( SCIPchgVarUbLazy(scip, var, 1.0));

                SCIP_CALL(SCIPaddVar(scip, var));

                z_vars[elem.first][elem.second] = var;
        }

        /*
       for commodity in range(num_commodities):
       NDSR_master.addConstrs( (z[i] >= 0 for i in list(arcs_0_dict.keys())),
       name="des_%d_"%(commodity))
       */

        //    There were two different set of constraints
        //    1) Design constarinst
        //    2) Path constraints
        std::vector<std::vector<std::vector<SCIP_CONS *>>> design_cons(num_commodities, std::vector<std::vector<SCIP_CONS *>>(num_nodes, std::vector<SCIP_CONS *>(num_nodes)));

        std::vector<SCIP_CONS *> path_cons(num_commodities);

        for (auto k = 0; k < num_commodities; ++k)
        {
                for (const auto &arc : list_of_arcs)
                {
                        SCIP_CONS *cons = nullptr;

                        namebuf.str("");
                        namebuf << "design_" << k << "_"
                                << "[" << arc.first << "," << arc.second << "]";
                        SCIP_CALL(SCIPcreateConsLinear(
                            scip, &cons, namebuf.str().c_str(), 0, nullptr, nullptr, 0.0, SCIPinfinity(scip), true, /* initial */
                            false,                                                                                  /* separate */
                            true,                                                                                   /* enforce */
                            true,                                                                                   /* check */
                            true,                                                                                   /* propagate */
                            false,                                                                                  /* local */
                            true,                                                                                   /* modifiable */
                            false,                                                                                  /* dynamic */
                            false,                                                                                  /* removable */
                            false /* stickingatnode */));

                        SCIP_CALL(SCIPaddCoefLinear(scip, cons, z_vars[arc.first][arc.second], 1.0));

                        SCIP_CALL(SCIPaddCons(scip, cons));

                        design_cons[k][arc.first][arc.second] = cons;
                }
        }

        /*
       for commodity in range(num_commodities):
       empty = LinExpr()
       NDSR_master.addConstr(empty==1,name="path_%d"%(commodity))
       */

        for (auto k = 0; k < num_commodities; ++k)
        {
                SCIP_CONS *cons = nullptr;
                namebuf.str("");
                namebuf << "path_" << k;

                SCIP_CALL(SCIPcreateConsLinear(
                    scip, &cons, namebuf.str().c_str(), 0, nullptr, nullptr, 1.0, 1.0, true, /* initial */
                    false,                                                                   /* separate */
                    true,                                                                    /* enforce */
                    true,                                                                    /* check */
                    true,                                                                    /* propagate */
                    false,                                                                   /* local */
                    true,                                                                    /* modifiable */
                    false,                                                                   /* dynamic */
                    false,                                                                   /* removable */
                    false /* stickingatnode */));

                SCIP_CALL(SCIPaddCons(scip, cons));

                path_cons[k] = cons;
        }

        // Let us add the path variables:

        std::vector<std::vector<SCIP_VAR *>> x_vars(num_commodities, std::vector<SCIP_VAR *>());

        std::vector<std::vector<int>> adj_list = rcspp_graph.get_adj_list();

        std::vector<std::vector<double>> w1_dist_matrix{};
        std::vector<std::vector<double>> w2_dist_matrix{};

        std::vector<std::vector<bool>> nodes_prunned_flag_per_commodity(num_commodities, std::vector<bool>(num_nodes, false)); // for each commpodty if a node has been pruned.

        for (int i = 0; i < num_nodes; i++)
        {
                auto dist_m1 = rcspp_graph.get_shortest_path_w1(i);
                w1_dist_matrix.push_back(dist_m1);

                auto dist_m2 = rcspp_graph.get_shortest_path_w2(i);
                w2_dist_matrix.push_back(dist_m2);
        }

        for (int k = 0; k < num_commodities; k++)
        {

                int max_m1_weight = comm_weight_info[k][0];
                int max_m2_weight = comm_weight_info[k][1];
                int source = comm_s_t_info[k].first;
                int terminal = comm_s_t_info[k].second;

                nodes_prunned_flag_per_commodity[k][source] = true; // the source node is technically pruned as we do not care about it.

                std::vector<double> v_m1(num_nodes, 0.0);
                std::vector<double> v_m2(num_nodes, 0.0);

                for (int i = 0; i < num_nodes; i++)
                {
                        v_m1[i] = w1_dist_matrix[source][i] + w1_dist_matrix[i][terminal];
                        v_m2[i] = w2_dist_matrix[source][i] + w2_dist_matrix[i][terminal];
                }
                for (int i = 0; i < num_nodes; i++)
                {
                        if (v_m1[i] > max_m1_weight or v_m2[i] > max_m2_weight)
                        {
                                nodes_prunned_flag_per_commodity[k][i] = true;
                        }
                }
        }

        auto best_RCSPP_commodities = get_rcspps_of_all_comms(rcspp_graph, nodes_prunned_flag_per_commodity, num_commodities, num_nodes, comm_weight_info, comm_s_t_info, w1_dist_matrix, w2_dist_matrix);

        for (auto k = 0; k < num_commodities; ++k)
        {
                auto path = best_RCSPP_commodities[k];

                SCIP_VAR *var = nullptr;
                namebuf.str("");
                namebuf << "x[" << k << "][" << num_paths_added_for_commodity[k] << "]";

                auto p_cost = instance.get_path_cost(k, path);

                SCIP_CALL(SCIPcreateVar(
                    scip, &var, namebuf.str().c_str(), 0.0, SCIPinfinity(scip), p_cost, SCIP_VARTYPE_CONTINUOUS,
                    true,  // initial
                    false, // forget the rest ...
                    nullptr, nullptr, nullptr, nullptr, nullptr));
                SCIP_CALL(SCIPaddVar(scip, var));
                x_vars[k].push_back(var);

                num_paths_added_for_commodity[k]++;
                paths_added_per_commodity[k].push_back(path);

                for (auto it = path.begin(); it != path.end() - 1; ++it)
                {
                        SCIP_CALL(SCIPaddCoefLinear(scip, design_cons[k][*it][*(it + 1)], var, -1.0));
                }

                SCIP_CALL(SCIPaddCoefLinear(scip, path_cons[k], var, 1.0));
        }

        std::cout << "Finish creating master: " << std::endl;


       

        const char *NDSR_PRICER_NAME = "NDSR_Pricer";

        ObjPricerNDSR *NDSR_pricer_ptr = new ObjPricerNDSR(
            scip, NDSR_PRICER_NAME, instance, z_vars, design_cons, path_cons, x_vars, num_paths_added_for_commodity, paths_added_per_commodity, num_commodities,
            list_of_arcs, num_nodes, num_arcs, num_metrics, arc_idx_dict, arc_metrics_vec, comm_weight_info, comm_s_t_info, variable_costs, rcspp_graph, nodes_prunned_flag_per_commodity, w1_dist_matrix, w2_dist_matrix);

        SCIP_CALL(SCIPincludeObjPricer(scip, NDSR_pricer_ptr, true));

        /* activate pricer */
        SCIP_CALL(SCIPactivatePricer(scip, SCIPfindPricer(scip, NDSR_PRICER_NAME)));

        /*
         ******************************************************************************************************************************************************************************************************************
         */

        SCIP_CALL(SCIPsetIntParam(scip, "display/verblevel", 4));
        SCIP_CALL(SCIPsetRealParam(scip, "limits/time", 3600));

        /*************
         *  Solve    *
         *************/

        SCIP_CALL(SCIPsolve(scip));

        int num_vars_added{0};
        for (const auto &paths_per_comm : paths_added_per_commodity)
        {
                num_vars_added += paths_per_comm.size();
        }

        std::cout << "Number of columns added: " << num_vars_added << "\n";

        /*Freeing the fixed arc variable*/

        for (auto &elem : instance.get_list_of_arcs())
        {
                SCIP_CALL(SCIPreleaseVar(scip, &z_vars[elem.first][elem.second]));
        }
        z_vars.clear();

        // Freeing the path vars x

        for (auto &comm : x_vars)
        {
                for (auto &xvar : comm)
                {
                        SCIP_CALL(SCIPreleaseVar(scip, &xvar));
                }
        }
        x_vars.clear();

        for (auto k = 0; k < num_commodities; ++k)
        {
                for (auto &arc : list_of_arcs)
                {
                        SCIP_CALL(SCIPreleaseCons(scip, &design_cons[k][arc.first][arc.second]));
                }
        }

        design_cons.clear();

        for (auto &constr : path_cons)
        {
                SCIP_CALL(SCIPreleaseCons(scip, &constr));
        }
        path_cons.clear();

        SCIP_CALL(SCIPfree(&scip));

        return SCIP_OKAY;
}

#endif /* NDSR_ColGen_Solver_h */
