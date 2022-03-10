#ifndef RCSPP_graph_HPP
#define RCSPP_graph_HPP

#include <iostream>
#include <queue>
#include <vector>

namespace RCSPP
{

class RCSPP_graph // begin declaration of the class
{
    public:                // begin public section
        RCSPP_graph () {}; // constructor

        RCSPP_graph ( int num_nodes )
        {
                m_adj_list.resize ( num_nodes );
                m_var_costs_matrix.resize ( num_nodes, std::vector<double> ( num_nodes, 1000000000000.0 ) );                 // just filling it with an arbitrary large number.
                m_w1_adjmatrix.resize ( num_nodes, std::vector<double> ( num_nodes, std::numeric_limits<double>::max () ) ); // just filling it with an arbitrary large number.
                m_w2_adjmatrix.resize ( num_nodes, std::vector<double> ( num_nodes, std::numeric_limits<double>::max () ) ); // just filling it with an arbitrary large number.
                m_num_nodes = num_nodes;
        };

        void add_arc ( int u, int v, double vcost, double w1, double w2 ); // adds a weighte arc (u,v) of weight w

        void delete_arc ( int u, int v );
        void delete_node ( int node );

        // std::vector<int> get_shortest_path ( int source, int sink );
        void set_const_graphs_weights ();

        void restore_graph ();

        bool has_arc ( int u, int v ); // a boolean function that returns true if an arc exists in the graph or not

        std::vector<int> get_neighbors ( int u )

        {
                return m_adj_list[u];
        }

        std::vector<std::vector<double>> get_w1_arc_vals ()
        {
                return m_w1_adjmatrix;
        }

        std::vector<std::vector<double>> get_w2_arc_vals ()
        {
                return m_w2_adjmatrix;
        }

        std::vector<std::vector<int>> get_adj_list ()
        {
                return m_adj_list;
        }
        std::vector<std::vector<double>> get_var_costs ()
        {
                return m_var_costs_matrix;
        }

        int get_num_nodes ()
        {
                return m_num_nodes;
        }

        std::vector<double> get_shortest_path_w1 ( const int source );
        std::vector<double> get_shortest_path_w2 ( const int source );

    private: // begin private section
        std::vector<std::vector<int>>    m_adj_list;
        std::vector<std::vector<double>> m_var_costs_matrix;
        std::vector<std::vector<double>> m_w1_adjmatrix;
        std::vector<std::vector<double>> m_w2_adjmatrix;
        std::vector<std::vector<double>> m_var_costs_matrix_cnst; // DO not touch this;
        int                              m_num_nodes;
};
}

#endif /* RCSPP_graph_HPP */