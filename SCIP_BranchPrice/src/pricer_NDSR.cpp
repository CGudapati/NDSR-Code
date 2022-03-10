/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2019 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not visit scip.zib.de.         */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   pricer_NDSR.cpp
 * @brief  NDSR variable pricer
 * @author Naga V. C. Gudapati
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include "pricer_NDSR.hpp"
#include <iostream>
#include <vector>

#include "scip/cons_linear.h"


#define PRICER_NAME "NDSR"
#define PRICER_DESC "variable pricer template"
#define PRICER_PRIORITY 0
#define PRICER_DELAY FALSE /* only call pricer if all problem variables have non-negative reduced costs */

/** Constructs the pricer object with the data needed
 *
 *  An alternative is to have a problem data class which allows to access the data.
 */
ObjPricerNDSR::ObjPricerNDSR(
    SCIP *scip, const char *p_name, NDSR_data &G, std::vector<std::vector<SCIP_VAR *>> p_z_vars, std::vector<std::vector<std::vector<SCIP_CONS *>>> p_design_cons, std::vector<SCIP_CONS *> p_path_cons,
         std::vector<std::vector<SCIP_VAR *>> &p_x_vars, std::vector<int> &p_num_paths_added_per_commodity,
    std::vector<std::vector<std::vector<int>>> &p_paths_added_per_commodity, const int p_num_commodities, const std::vector<std::pair<int, int>> &p_list_of_arcs, const int p_num_nodes, const int p_num_arcs, const int p_num_metrics, const std::vector<std::vector<int>> &p_arc_idx_dict,
    const std::vector<std::vector<std::vector<std::vector<double>>>> &p_arc_metrics_vec, const std::vector<std::vector<double>> &p_commodity_weights_info, const std::vector<std::pair<int, int>> &p_commodity_s_t_info, const std::vector<std::vector<std::vector<double>>> &p_variable_costs_vec,
    RCSPP::RCSPP_graph &rcspp_graph, std::vector<std::vector<bool>> &p_nodes_prunned_flag_per_commodity, std::vector<std::vector<double>> &p_w1_dist_matrix, std::vector<std::vector<double>> &p_w2_dist_matrix)
    : ObjPricer(scip, p_name, "Finds Path with negative reduced cost.", 0, TRUE), _G(G), _z_vars(p_z_vars), _design_cons(p_design_cons), _path_cons(p_path_cons),  _x_vars(p_x_vars), _num_paths_added_per_commodity(p_num_paths_added_per_commodity), _paths_added_per_commodity(p_paths_added_per_commodity), _num_commodities(p_num_commodities), _list_of_arcs(p_list_of_arcs), _num_nodes(p_num_nodes), _num_arcs(p_num_arcs), _commodity_weights_info(p_commodity_weights_info), _commodity_s_t_info(p_commodity_s_t_info), _variable_costs_vec(p_variable_costs_vec), _rcspp_time(0.0), _num_RCSPP_calls(0), _num_cols_added_in_pricer(0), m_rcspp_graph(rcspp_graph), m_nodes_prunned_flag_per_commodity(p_nodes_prunned_flag_per_commodity), m_w1_dist_matrix(p_w1_dist_matrix), m_w2_dist_matrix(p_w2_dist_matrix)

{
        _var_costs_per_comm.resize(p_num_commodities);
};

/** Destructs the pricer object. */
ObjPricerNDSR::~ObjPricerNDSR()
{
}

SCIP_DECL_PRICEREXITSOL(ObjPricerNDSR::scip_exitsol)
{

        std::cout << "\nelapsed rcspp time is: " << _rcspp_time / 1000.0 << "\n";
        std::cout << "num calls to RCSPP are: " << _num_RCSPP_calls << "\n";
        std::cout << "num cols added in pricer: " << _num_cols_added_in_pricer << "\n";

        return SCIP_OKAY;
}

SCIP_DECL_PRICERINIT(ObjPricerNDSR::scip_init)
{

        for (int k = 0; k < _num_commodities; ++k)
        {
                for (auto &arc : _list_of_arcs)
                {
                        auto cost = _variable_costs_vec[k][arc.first][arc.second];
                        _var_costs_per_comm[k].push_back(cost);
                }
        }

        for (auto k = 0; k < _num_commodities; ++k)
        {
                for (const auto &arc : _list_of_arcs)
                {
                        SCIP_CALL(SCIPgetTransformedCons(scip, _design_cons[k][arc.first][arc.second], &_design_cons[k][arc.first][arc.second]));
                }
        }

        for (auto k = 0; k < _num_commodities; ++k)
        {
                SCIP_CALL(SCIPgetTransformedCons(scip, _path_cons[k], &_path_cons[k]));
        }

        // getting the transofromed z_vars and X_vars

        for (auto &elem : _list_of_arcs)
        {
                SCIP_CALL(SCIPgetTransformedVar(scip, _z_vars[elem.first][elem.second], &_z_vars[elem.first][elem.second]));
        }

        /* TODO: Fix the _x_var referencing*/

        for (auto comm : _x_vars)
        {
                for (auto xvar : comm)
                {
                        SCIP_CALL(SCIPgetTransformedVar(scip, xvar, &xvar));
                }
        }

        return SCIP_OKAY;
} 

SCIP_RETCODE ObjPricerNDSR::pricing(
    SCIP *scip,   /**< SCIP data structure */
    bool isfarkas /**< whether we perform Farkas pricing */
)
{

        std::vector<double> dual_price_pi{};

        for (auto k = 0; k < _num_commodities; ++k)
        {
                for (const auto &arc : _list_of_arcs)
                {
                        dual_price_pi.push_back(SCIPgetDualsolLinear(scip, _design_cons[k][arc.first][arc.second]));
                }
        }

        for (auto k = 0; k < _num_commodities; ++k)
        {
                dual_price_pi.push_back(SCIPgetDualsolLinear(scip, _path_cons[k]));
        }

        auto dual_prices_rho = std::vector<double>(dual_price_pi.begin() + _num_commodities * _num_arcs, dual_price_pi.end());

        std::vector<int> path_to_be_added{};

        auto var_costs_per_all_k = _variable_costs_vec; // for the RCSPP_MM_pricer

        for (auto k = 0; k < _num_commodities; ++k)
        {

                auto dual_prices_gamma = std::vector<double>(dual_price_pi.begin() + k * (_num_arcs), dual_price_pi.begin() + k * _num_arcs + _num_arcs); // this is dual_vars_pi

                std::vector<double> dual_prices_phi(_num_arcs, 0.0);


                for (const auto &arc : _list_of_arcs)
                {
                        auto idx_of_arc = &arc - &_list_of_arcs[0];
                        var_costs_per_all_k[k][arc.first][arc.second] += dual_prices_gamma[idx_of_arc] + dual_prices_phi[idx_of_arc];
                }
        }

        auto start_time = std::chrono::steady_clock::now();

        auto rcspp_cost_paths = get_rcspps_of_all_comms_in_pricer(_G, m_rcspp_graph, m_nodes_prunned_flag_per_commodity, _num_commodities, _num_nodes, _commodity_weights_info, _commodity_s_t_info, m_w1_dist_matrix, m_w2_dist_matrix, var_costs_per_all_k, dual_prices_rho);
        _num_RCSPP_calls++;
        auto end_time = std::chrono::steady_clock::now();


        _rcspp_time += std::chrono::duration<double, std::milli>(end_time - start_time).count();

        auto rcspp_paths = rcspp_cost_paths.first;
        auto rcspp_costs = rcspp_cost_paths.second;

        for (int k = 0; k < _num_commodities; ++k)
        {

                if (rcspp_costs[k] - dual_prices_rho[k] < -0.001)
                {
                        auto path_cost = _G.get_path_cost(k, rcspp_paths[k]);
                        add_path_variable(scip, rcspp_paths[k], k, path_cost);
                        _num_cols_added_in_pricer++;
                }
        }

        return SCIP_OKAY;
}

SCIP_DECL_PRICERREDCOST(ObjPricerNDSR::scip_redcost)
{
        SCIPdebugMsg(scip, "call scip_redcost ...\n");

        /* set result pointer, see above */
        *result = SCIP_SUCCESS;

        /* call pricing routine */
        SCIP_CALL(pricing(scip, false));

        return SCIP_OKAY;
} 

SCIP_DECL_PRICERFARKAS(ObjPricerNDSR::scip_farkas)
{
        SCIPdebugMsg(scip, "call scip_farkas ...\n");

        /* call pricing routine */
        SCIP_CALL(pricing(scip, true));

        return SCIP_OKAY;
} 

/** add tour variable to problem */
SCIP_RETCODE ObjPricerNDSR::add_path_variable(
    SCIP *scip,                   /**< SCIP data structure */
    const std::vector<int> &path, /**< list of nodes in tour */
    int commodity_index,
    double p_cost // path cost
)
{
        std::ostringstream namebuffer;
        namebuffer.str("");
        namebuffer << "x[" << commodity_index << "][" << _num_paths_added_per_commodity[commodity_index] << "]";


        // Create the new variable and add it to the SCIP

        SCIP_VAR *var;
        SCIP_CALL(SCIPcreateVar(
            scip, &var, namebuffer.str().c_str(),
            0.0,                     // lower bound
            SCIPinfinity(scip),      // upper bound
            p_cost,                  // objective
            SCIP_VARTYPE_CONTINUOUS, // variable type
            false, false, NULL, NULL, NULL, NULL, NULL));

        //    /* add new variable to the list of variables to price into LP (score: leave 1 here) */
        SCIP_CALL(SCIPaddPricedVar(scip, var, 1.0));

        // Adding the priced variable to the design constraints
        for (auto it = path.begin(); it != path.end() - 1; ++it)
        {
                SCIP_CALL(SCIPaddCoefLinear(scip, _design_cons[commodity_index][*it][*(it + 1)], var, -1.0));
        }

        // Ading the priced variable to the path constraints
        SCIP_CALL(SCIPaddCoefLinear(scip, _path_cons[commodity_index], var, 1.0));

        _x_vars[commodity_index].push_back(var);
        _num_paths_added_per_commodity[commodity_index]++;
        _paths_added_per_commodity[commodity_index].push_back(path);

        return SCIP_OKAY;
}
