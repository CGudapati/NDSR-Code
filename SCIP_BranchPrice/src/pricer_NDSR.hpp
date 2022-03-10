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

/**@file   pricer_NDSR.h
 * @ingroup PRICERS
 * @brief  NDSR variable pricer
 * @author Naga V. C. Gudapati
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __SCIP_PRICER_NDSR_H__
#define __SCIP_PRICER_NDSR_H__

#include "NDSR_Instance.hpp"
#include "RCSPP_graph.hpp"
#include "Timer.hpp"
#include "get_RCSPP_for_all_comm_pricer.h"
#include "objscip/objscip.h"
#include "scip/pub_var.h"
#include <fstream>
#include <sstream>
#include <vector>

using cycles_per_cut_1_CUT_IF = std::vector<std::vector<std::pair<int, int>>>;
using cycles_per_cut_3_OR     = std::vector<std::vector<std::pair<int, int>>>;

class ObjPricerNDSR : public scip::ObjPricer
{
    public:
        ObjPricerNDSR(
            SCIP* scip, const char* p_name, NDSR_data& G, std::vector<std::vector<SCIP_VAR*>> p_z_vars, std::vector<std::vector<std::vector<SCIP_CONS*>>> p_design_cons, std::vector<SCIP_CONS*> p_path_cons,  std::vector<std::vector<SCIP_VAR*>>& p_x_vars,
            std::vector<int>& p_num_paths_added_per_commodity, std::vector<std::vector<std::vector<int>>>& p_paths_added_per_commodity,  const int p_num_commodities, const std::vector<std::pair<int, int>>& p_list_of_arcs, const int p_num_nodes, const int p_num_arcs,
            const int p_num_metrics, const std::vector<std::vector<int>>& p_arc_idx_dict, const std::vector<std::vector<std::vector<std::vector<double>>>>& p_arc_metrics_vec, const std::vector<std::vector<double>>& p_commodity_weights_info,
            const std::vector<std::pair<int, int>>& p_commodity_s_t_info, const std::vector<std::vector<std::vector<double>>>& p_variable_costs_vec, RCSPP::RCSPP_graph& rcspp_graph, std::vector<std::vector<bool>>& p_nodes_prunned_flag_per_commodity,
            std::vector<std::vector<double>>& p_w1_dist_matrix, std::vector<std::vector<double>>& p_w2_dist_matrix);

    public:
        /** Destructs the pricer object. */
        virtual ~ObjPricerNDSR();

        /** initialization method of variable pricer (called after problem was transformed) */
        virtual SCIP_DECL_PRICERINIT(scip_init);

        /** reduced cost pricing method of variable pricer for feasible LPs */
        virtual SCIP_DECL_PRICERREDCOST(scip_redcost);

        /** farkas pricing method of variable pricer for infeasible LPs */
        virtual SCIP_DECL_PRICERFARKAS(scip_farkas);

        virtual SCIP_DECL_PRICEREXITSOL(scip_exitsol);

        /** perform pricing */
        SCIP_RETCODE pricing(
            SCIP* scip,    /**< SCIP data structure */
            bool  isfarkas /**< whether we perform Farkas pricing */
        );

        /** add tour variable to problem */
        SCIP_RETCODE add_path_variable(
            SCIP*                   scip, /**< SCIP data structure */
            const std::vector<int>& path, /**< list of nodes in tour */
            int                     commodity_index,
            double                  p_cost // path cost
        );

    private:
        NDSR_data&                                        _G;
        std::vector<std::vector<SCIP_VAR*>>               _z_vars;
        std::vector<std::vector<std::vector<SCIP_CONS*>>> _design_cons;
        std::vector<SCIP_CONS*>                           _path_cons;
        std::vector<std::vector<SCIP_VAR*>>&              _x_vars;
        std::vector<int>&                                 _num_paths_added_per_commodity;
        std::vector<std::vector<std::vector<int>>>&       _paths_added_per_commodity;
        const int                                                         _num_commodities;
        const std::vector<std::pair<int, int>>&                           _list_of_arcs;
        const int                                                         _num_nodes;
        const int                                                         _num_arcs;
        const std::vector<std::vector<double>>&                           _commodity_weights_info;
        const std::vector<std::pair<int, int>>&                           _commodity_s_t_info;
        const std::vector<std::vector<std::vector<double>>>&              _variable_costs_vec;
        std::vector<std::vector<double>>                                  _var_costs_per_comm;
        double                                                            _rcspp_time;
        int                                                               _num_RCSPP_calls;
        double                                                            _num_cols_added_in_pricer;
        RCSPP::RCSPP_graph&                                               m_rcspp_graph;
        std::vector<std::vector<bool>>&                                   m_nodes_prunned_flag_per_commodity;
        std::vector<std::vector<double>>&                                 m_w1_dist_matrix;
        std::vector<std::vector<double>>&                                 m_w2_dist_matrix;
};

/** creates the NDSR variable pricer and includes it in SCIP
 *
 *  @ingroup PricerIncludes
 */

#endif
