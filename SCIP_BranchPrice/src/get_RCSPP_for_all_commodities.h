#ifndef GET_RCSPP_FOR_ALL_COMMODITIES_H
#define GET_RCSPP_FOR_ALL_COMMODITIES_H

#include "NDSR_Reader.h"
#include "Nodes_and_labels.h"
#include "RCSPP_graph.hpp"
#include "helper.h"
#include <chrono>
#include <vector>

inline std::vector<std::vector<int>> get_rcspps_of_all_comms(
    RCSPP::RCSPP_graph& rcspp_graph, std::vector<std::vector<bool>>& nodes_prunned_flag_per_commodity, int num_commodities, int num_nodes, const std::vector<std::vector<double>>& comm_weight_info, const std::vector<std::pair<int, int>>& comm_s_t_info,
    std::vector<std::vector<double>>& w1_dist_matrix, std::vector<std::vector<double>>& w2_dist_matrix)
{

        std::vector<std::vector<int>> rcspp_all_comms {};

        rcspp_all_comms.resize(num_commodities);

        for (int k = 0; k < num_commodities; ++k)
        {

                int max_m1_weight = comm_weight_info[k][0];
                int max_m2_weight = comm_weight_info[k][1];
                int source        = comm_s_t_info[k].first;
                int terminal      = comm_s_t_info[k].second;

                // Let us get the RCSPPs of a commodity:

                std::vector<Node*> list_of_nodes {};

                list_of_nodes.resize(num_nodes);

                for (int i = 0; i < num_nodes; ++i)
                {
                        Node* n          = new Node();
                        n->m_node_idx    = i;
                        list_of_nodes[i] = n;
                }

                Label* lab = new Label(source, nullptr, 0, 0, 0, true);
                list_of_nodes[source]->m_labels.push_back(lab);
                list_of_nodes[source]->m_num_labels++;

                std::stack<Node> node_stk {};
                vector_set<int>  node_stk_elems {};

                node_stk.push(*list_of_nodes[source]);
                node_stk_elems.insert(list_of_nodes[source]->m_node_idx);

                auto w1_adj        = rcspp_graph.get_w1_arc_vals();
                auto w2_adj        = rcspp_graph.get_w2_arc_vals();
                auto var_costs_adj = rcspp_graph.get_var_costs();

                while (!node_stk.empty())
                {

                        Node u = node_stk.top();
                        node_stk.pop();
                        node_stk_elems.erase_elem(u.m_node_idx);


                        std::vector<int> neighbors = rcspp_graph.get_neighbors(u.m_node_idx);

                        auto num_labels = list_of_nodes[u.m_node_idx]->m_num_labels;

                        for (int i = 0; i < num_labels; ++i)
                        {
                                bool to_be_expanded = false;

                                Label* ll = list_of_nodes[u.m_node_idx]->m_labels[i];

                                for (auto& v : neighbors)
                                {
                                        if (!nodes_prunned_flag_per_commodity[k][v])
                                        {
                                                if (ll->m_flag == false)
                                                {
                                                        continue;
                                                }

                                                if (ll->m_m1_cons + w1_adj[u.m_node_idx][v] + w1_dist_matrix[v][terminal] > max_m1_weight)
                                                {
                                                        continue;
                                                }

                                                else if (ll->m_m2_cons + w2_adj[u.m_node_idx][v] + w2_dist_matrix[v][terminal] > max_m2_weight)
                                                {
                                                        continue;
                                                }

                                                else if (is_v_in_l(v, ll, source))
                                                {
                                                        continue;
                                                }
                                                else if (dominated(u.m_node_idx, list_of_nodes[v], ll, var_costs_adj, w1_adj, w2_adj))
                                                {
                                                        // std::cout << "Dominated!" << std::endl;

                                                        continue;
                                                }
                                                else
                                                {
                                                        to_be_expanded = true;
                                                        Label* lp      = new Label(v, ll, ll->m_cost + var_costs_adj[u.m_node_idx][v], ll->m_m1_cons + w1_adj[u.m_node_idx][v], ll->m_m2_cons + w2_adj[u.m_node_idx][v], true);
                                                        list_of_nodes[v]->m_num_labels++;
                                                        list_of_nodes[v]->m_labels.push_back(lp);
                                                }

                                                if (to_be_expanded and v != terminal and !node_stk_elems.has_elem(v))
                                                {
                                                        node_stk.push(*list_of_nodes[v]);
                                                        node_stk_elems.insert(v);
                                                }
                                        }
                                }

                                ll->m_flag = false;
                        }
                }


                std::vector<std::vector<int>> list_of_feasible_paths {};
                std::vector<double>           list_of_path_costs {};

                std::vector<int> best_path {};
                double           best_path_cst = std::numeric_limits<double>::max();

                for (int i = 0; i < list_of_nodes[terminal]->m_num_labels; ++i)
                {
                        std::vector<int> pth {};
                        auto             label       = list_of_nodes[terminal]->m_labels[i];
                        auto             path_cost = label->m_cost;

                        list_of_path_costs.push_back(path_cost);

                        auto current = label->m_node_idx;
                        pth.push_back(current);

                        while (current != source)
                        {
                                label     = label->m_pred_label;
                                current = label->m_node_idx;
                                pth.push_back(current);
                        }

                        std::reverse(pth.begin(), pth.end());
                        list_of_feasible_paths.push_back(pth);
                        if (path_cost < best_path_cst)
                        {
                                best_path     = pth;
                                best_path_cst = path_cost;
                        }
                }
                
                rcspp_all_comms[k] = best_path;

                // Clean up of memory
                for (int i = 0; i < num_nodes; ++i)
                {
                        for (int j = 0; j < list_of_nodes[i]->m_num_labels; ++j)
                        {
                                if (list_of_nodes[i]->m_labels[j])
                                {
                                        delete list_of_nodes[i]->m_labels[j];
                                }
                        }

                        delete list_of_nodes[i];
                }
        }

        return rcspp_all_comms;
}

#endif /* GET_RCSPP_FOR_ALL_COMMODITIES_H */
