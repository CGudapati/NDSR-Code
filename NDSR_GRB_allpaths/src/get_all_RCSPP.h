#ifndef GET_ALL_RCSPP_H
#define GET_ALL_RCSPP_H

#include "Graph_Reader.h"
#include "NDSR_Reader.h"
#include "RCSPP_graph.hpp"
#include "helper.h"
#include <iostream>
#include <set>
#include <stack>
#include <string>

struct Label
{
        int    m_node_idx;
        Label* m_pred_label = nullptr;
        double m_cost;
        double m_m1_cons;
        double m_m2_cons;
        bool   m_flag;
        Label(int node_id, Label* label, double cost, double m1_cons, double m2_cons, bool flag)
        {
                m_node_idx   = node_id;
                m_pred_label = label;
                m_cost       = cost;
                m_m1_cons    = m1_cons;
                m_m2_cons    = m2_cons;
                m_flag       = flag;
        }
        ~Label()
        {
                // delete m_pred;
        }
};

struct Node
{
        int                 m_node_idx;
        int                 m_num_labels { 0 };
        std::vector<Label*> m_labels {};
        Node()
        {
                m_node_idx   = -1;
                m_num_labels = 0;
                m_labels     = {};
        }

        ~Node() {};
};

bool is_v_in_l(int v, Label* lab, int s)
{
        auto current = lab->m_node_idx;

        while (current != s)
        {
                if (current == v)
                {
                        return true;
                }
                lab     = lab->m_pred_label;
                current = lab->m_node_idx;
        }
        return false;
}

std::vector<std::set<std::vector<int>>> get_rcspps_of_all_comms(NDSR_data& G, RCSPP::RCSPP_graph& RCSPP_G)
{

        const auto num_commodities  = G.get_num_commodities();
        const auto num_nodes        = G.get_num_nodes();
        const auto comm_s_t_info    = G.get_commodity_s_t_info();
        const auto comm_weight_info = G.get_commodity_weights_info();

        std::vector<std::set<std::vector<int>>> rcspp_all_comms {};

        for (int k = 0; k < num_commodities; ++k)
        {

                int max_m1_weight = comm_weight_info[k][0];
                int max_m2_weight = comm_weight_info[k][1];
                int source        = comm_s_t_info[k].first;
                int terminal      = comm_s_t_info[k].second;

                std::vector<std::vector<int>> adj_list = RCSPP_G.get_adj_list();

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

                auto w1_adj        = RCSPP_G.get_w1_arc_vals();
                auto w2_adj        = RCSPP_G.get_w2_arc_vals();
                auto var_costs_adj = RCSPP_G.get_var_costs();

                while (!node_stk.empty())
                {
                        Node u = node_stk.top();
                        node_stk.pop();
                        node_stk_elems.erase_elem(u.m_node_idx);


                        std::vector<int> neighbors = RCSPP_G.get_neighbors(u.m_node_idx);

                        auto num_labels = list_of_nodes[u.m_node_idx]->m_num_labels;

                        for (int i = 0; i < num_labels; ++i)
                        {
                                bool to_be_expanded = false;

                                Label* ll = list_of_nodes[u.m_node_idx]->m_labels[i];
                                for (auto& v : neighbors)
                                {
                                        if (v != source)
                                        {
                                                if (ll->m_flag == false)
                                                {
                                                        continue;
                                                }

                                                if (ll->m_m1_cons + w1_adj[u.m_node_idx][v] > max_m1_weight)
                                                {
                                                        continue;
                                                }

                                                else if (ll->m_m2_cons + w2_adj[u.m_node_idx][v] > max_m2_weight)
                                                {

                                                        continue;
                                                }

                                                else if (is_v_in_l(v, ll, source))
                                                {

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

                for (int i = 0; i < list_of_nodes[terminal]->m_num_labels; ++i)
                {
                        std::vector<int> pth {};
                        auto             lab = list_of_nodes[terminal]->m_labels[i];
                        list_of_path_costs.push_back(lab->m_cost);

                        auto current = lab->m_node_idx;
                        pth.push_back(current);

                        while (current != source)
                        {
                                lab     = lab->m_pred_label;
                                current = lab->m_node_idx;
                                pth.push_back(current);
                        }

                        std::reverse(pth.begin(), pth.end());
                        list_of_feasible_paths.push_back(pth);
                }

                std::set<std::vector<int>> list_of_unique_paths {};

                for (const auto& path : list_of_feasible_paths)
                {

                        list_of_unique_paths.insert(path);
                }
                rcspp_all_comms.push_back(list_of_unique_paths);

                list_of_feasible_paths.clear();
                list_of_unique_paths.clear();

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
        std::cout << "finished find all the RCSPPs!" << std::endl;
        auto total_num_paths { 0 };
        for (int i = 0; i < rcspp_all_comms.size(); ++i)
        {
                std::cout << "commodity " << i << " has " << rcspp_all_comms[i].size() << " paths added\n";
                total_num_paths += rcspp_all_comms[i].size();
        }
        std::cout << "******** Total Number of paths: " << total_num_paths << " ********\n";
        return rcspp_all_comms;
}
#endif /* GET_ALL_RCSPP_H */
