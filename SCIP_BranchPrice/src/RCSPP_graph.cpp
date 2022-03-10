#include "RCSPP_graph.hpp"

void RCSPP::RCSPP_graph::add_arc ( int u, int v, double vcost, double w1, double w2 )
{
        m_adj_list[u].push_back ( v );
        m_var_costs_matrix[u][v] = vcost;
        m_w1_adjmatrix[u][v]     = w1;
        m_w2_adjmatrix[u][v]     = w2;
}

void RCSPP::RCSPP_graph::delete_arc ( int u, int v )
{
        m_var_costs_matrix[u][v] = std::numeric_limits<double>::max ();
}

void RCSPP::RCSPP_graph::delete_node ( int node )
{

        for ( int i = 0; i < m_num_nodes; ++i )
        {
                m_var_costs_matrix[node][i] = std::numeric_limits<double>::max ();
                m_var_costs_matrix[i][node] = std::numeric_limits<double>::max ();
        }
}

void RCSPP::RCSPP_graph::restore_graph ()
{
        m_var_costs_matrix = m_var_costs_matrix_cnst;
}

void RCSPP::RCSPP_graph::set_const_graphs_weights () // This should only ever be called once.
{
        std::cout << "We are setting the const costs graphs\n";

        m_var_costs_matrix_cnst = m_var_costs_matrix;
}

bool RCSPP::RCSPP_graph::has_arc ( int u, int v )
{
        if ( m_var_costs_matrix[u][v] < 100000000.0 ) // I do not like this but it should be okay as we do not expect the variable costs to ever go higher
        {
                return true;
        }
        else
        {
                return false;
        }
}


std::vector<double> RCSPP::RCSPP_graph::get_shortest_path_w1 ( const int source )
{


        std::vector<int> prev ( m_num_nodes, std::numeric_limits<int>::max () );

        prev[source] = -1;

        std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<std::pair<double, int>>> pq; // Just a priority queue


        std::vector<double> dist ( m_num_nodes, std::numeric_limits<double>::max () );


        pq.push ( std::make_pair ( 0.0, source ) );

        dist[source] = 0.0;

        
        while ( !pq.empty () )
        {
                int u = pq.top ().second;
                pq.pop ();

        
                for ( auto v : RCSPP::RCSPP_graph::m_adj_list[u] )
                {
                        double weight = m_w1_adjmatrix[u][v];
        
                        if ( dist[v] > dist[u] + weight )
                        {
        
                                dist[v] = dist[u] + weight;
                                pq.push ( std::make_pair ( dist[v], v ) );
                                prev[v] = u;
                        }
                }
        }

        
        return dist;
}

std::vector<double> RCSPP::RCSPP_graph::get_shortest_path_w2 ( const int source )
{
        

        std::vector<int> prev ( m_num_nodes, std::numeric_limits<int>::max () );

        prev[source] = -1;

        std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<std::pair<double, int>>> pq; // Just a priority queue

        
        std::vector<double> dist ( m_num_nodes, std::numeric_limits<double>::max () );

        
        pq.push ( std::make_pair ( 0.0, source ) );

        dist[source] = 0.0;

        
        while ( !pq.empty () )
        {
                
                int u = pq.top ().second;
                pq.pop ();

                
                for ( auto v : RCSPP::RCSPP_graph::m_adj_list[u] )
                {
                        
                        double weight = m_w2_adjmatrix[u][v];
                        
                        if ( dist[v] > dist[u] + weight )
                        {
                        
                                dist[v] = dist[u] + weight;
                                pq.push ( std::make_pair ( dist[v], v ) );
                                prev[v] = u;
                        }
                }
        }

        return dist;
}