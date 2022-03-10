#include "Graph_Reader.h"
#include "NDSR_Reader.h"
#include "Nodes_and_labels.h"
#include "RCSPP_graph.hpp"
#include "gurobi_c++.h"
#include <iostream>
#include <stack>
#include <string>

#include <chrono>

int main(int argc, const char *argv[])
{

        std::string filename = argv[1];   // data set
        std::string multiplier = "1.00";
        
        double weight_multiplier = 1.00;  // this is the default weight multiplier. If we increase the weight, then the number of feasible paths increases.
        if (argc > 2)
        {
                multiplier = argv[2]; //
        }
        
        
        if (argc > 2)
        {
                weight_multiplier = std::stod(multiplier);
        }

        auto inst = read_instance_NDSR(filename, weight_multiplier); // We read the NDSR instance and store it in the inst variable.

        // We read the same file again to creta the graph to solve the resource constrained shortest path problme.
        // We do it twice because this code eveloved a lot and i thought this was easier.
        auto rcspp_graph = read_instance_RCSPP_graph(filename, weight_multiplier);

        // just getting some stuff that will be used again and again.
        // the get() call is very expensive. so we store those values in some varaibles
        const auto num_commodities = inst.get_num_commodities();
        const auto num_nodes = inst.get_num_nodes();
        const auto comm_s_t_info = inst.get_commodity_s_t_info();
        const auto comm_weight_info = inst.get_commodity_weights_info();

        // Start measuring time
        auto begin_preprocessing = std::chrono::steady_clock::now();

        // So each could hypothetically have a different underlying graph. we we have store "k" number of adjacencly lists
        // each adjacecny list is a vector of vectors.
        std::vector<std::vector<int>> adj_list = rcspp_graph.get_adj_list();

        // we have two weight metrics as in we have two resource oonstarints.
        // So we are constructing two data matrices.
        // each matrix will have the its weights of variaous arcs as the distance values.
        // We can use these distances to prune certain undersirable nodes where we know for a fact that the paths will not visit those nodes
        // for a commodity.

        std::vector<std::vector<double>> w1_dist_matrix{}; // This will have the shortestpath distances from one node to all othernodes.
        std::vector<std::vector<double>> w2_dist_matrix{}; // same but for weight w2 as diatnce

        std::vector<std::vector<bool>> nodes_prunned_flag_per_commodity(num_commodities, std::vector<bool>(num_nodes, false)); // for each commpodty if a node has been pruned.

        for (int i = 0; i < num_nodes; i++)
        {
                auto dist_m1 = rcspp_graph.get_shortest_path_w1(i);
                w1_dist_matrix.push_back(dist_m1);

                auto dist_m2 = rcspp_graph.get_shortest_path_w2(i);
                w2_dist_matrix.push_back(dist_m2);
        }

        // for each commodity and for each node we will see if that node is a good node.
        // basically if the shortest path from the source to that node + shortest path from that node to the termainal
        // is greater than the weight limit, then that node can be pruned for that commodity
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

        auto end_preprocessing = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_preprocessing - begin_preprocessing);

        std::cout << "Time taken for preprocessing (s): " << elapsed.count() * 1e-6 << std::endl;

        // below we enumerate all the paths. the algorithm is here https://arxiv.org/abs/2107.01101

        auto begin_rcspp_all = std::chrono::steady_clock::now();

        std::vector<std::vector<std::vector<int>>> rcspp_all_comms{};

        rcspp_all_comms.resize(num_commodities);

        for (int k = 0; k < num_commodities; ++k)
        {

                int max_m1_weight = comm_weight_info[k][0];
                int max_m2_weight = comm_weight_info[k][1];
                int source = comm_s_t_info[k].first;
                int terminal = comm_s_t_info[k].second;

                std::vector<Node *> list_of_nodes{};

                list_of_nodes.resize(num_nodes);

                for (int i = 0; i < num_nodes; ++i)
                {
                        Node *n = new Node();
                        n->m_node_idx = i;
                        list_of_nodes[i] = n;
                }

                Label *lab = new Label(source, nullptr, 0, 0, 0, true);
                list_of_nodes[source]->m_labels.push_back(lab);
                list_of_nodes[source]->m_num_labels++;

                std::stack<Node> node_stk{};
                vector_set<int> node_stk_elems{};

                node_stk.push(*list_of_nodes[source]);
                node_stk_elems.insert(list_of_nodes[source]->m_node_idx);

                auto w1_adj = rcspp_graph.get_w1_arc_vals();
                auto w2_adj = rcspp_graph.get_w2_arc_vals();
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

                                Label *ll = list_of_nodes[u.m_node_idx]->m_labels[i];

                                for (auto &v : neighbors)
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

                                                else
                                                {
                                                        to_be_expanded = true;
                                                        Label *lp = new Label(v, ll, ll->m_cost + var_costs_adj[u.m_node_idx][v], ll->m_m1_cons + w1_adj[u.m_node_idx][v], ll->m_m2_cons + w2_adj[u.m_node_idx][v], true);
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
                        // std::cout << std::endl << std::std::endl;
                }

                std::vector<std::vector<int>> list_of_feasible_paths{};
                std::vector<double> list_of_path_costs{};

                for (int i = 0; i < list_of_nodes[terminal]->m_num_labels; ++i)
                {
                        std::vector<int> pth{};
                        auto lab = list_of_nodes[terminal]->m_labels[i];
                        auto path_cost = lab->m_cost;

                        list_of_path_costs.push_back(path_cost);

                        auto current = lab->m_node_idx;
                        pth.push_back(current);

                        while (current != source)
                        {
                                lab = lab->m_pred_label;
                                current = lab->m_node_idx;
                                pth.push_back(current);
                        }

                        std::reverse(pth.begin(), pth.end());
                        list_of_feasible_paths.push_back(pth);
                }

                rcspp_all_comms[k] = list_of_feasible_paths;

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

        auto end_all_RCSPP = std::chrono::steady_clock::now();
        auto elapsed_all_rcspp = std::chrono::duration_cast<std::chrono::microseconds>(end_all_RCSPP - begin_rcspp_all);

        // std::cout << "Time taken to find all RCSPP: " << elapsed_all_rcspp.count() * 1e-6 << " s\n";

        std::cout << "Total time (s): " << (elapsed_all_rcspp.count() * 1e-6) + (elapsed.count() * 1e-6) << "\n";

        // std::cout << "finished find all the RCSPPs!" << std::std::endl;
        auto total_num_paths{0};
        for (int i = 0; i < rcspp_all_comms.size(); ++i)
        {
                total_num_paths += rcspp_all_comms[i].size();
        }
        std::cout << "******** Total Number of paths: " << total_num_paths << " ********\n";

        // here we add all path to the the gurobi model.
        try
        {

                // Create an environment
                GRBEnv env = GRBEnv(true);
                env.set("Threads", "1");
                env.set("TimeLimit", "3600");
                env.set("MIPGap", "0.0");
                env.set("IntFeasTol", "1e-9");
                env.set("LogFile", "");
                env.start();

                // Create an empty model
                GRBModel model = GRBModel(env);

                // Create variables

                const auto list_of_arcs = inst.get_list_of_arcs();
                auto fixed_costs = inst.get_fixed_costs();

                // We are adding the z vars
                std::vector<GRBVar> z_vars{};
                z_vars.resize(inst.get_num_arcs());

                for (auto &elem : list_of_arcs)
                {
                        auto arc_idx = &elem - &list_of_arcs[0];
                        z_vars[arc_idx] = model.addVar(0.0, 1.0, fixed_costs[elem.first][elem.second], GRB_BINARY, "z[" + std::to_string(elem.first) + "][" + std::to_string(elem.second) + "]");
                }

                // Getting the xvars ready
                std::vector<std::vector<GRBVar>> x_vars; // These are the path varaibles;
                x_vars.resize(num_commodities);

                for (auto &set_of_paths : rcspp_all_comms)
                {
                        auto k = &set_of_paths - &rcspp_all_comms[0];
                        // std::cout << "comm idx: " << k << "\n";
                        for (auto &path : set_of_paths)
                        {
                                std::vector<int> pth = path;
                                auto idx_of_path = &path - &set_of_paths[0];
                                auto p_cost = inst.get_path_cost(k, pth);
                                x_vars[k].push_back(model.addVar(0.0, 1.0, p_cost, GRB_CONTINUOUS, "x[" + std::to_string(k) + "][" + std::to_string(idx_of_path) + "]"));
                        }
                }

                GRBLinExpr expr = 0;
                std::ostringstream namebuf{};

                std::vector<GRBConstr> path_cons(num_commodities);

                for (auto k = 0; k < num_commodities; ++k)
                {
                        expr = 0;

                        for (auto &path_var : x_vars[k])
                        {
                                expr += path_var;
                        }

                        namebuf.str("");
                        namebuf << "path_" << k;

                        path_cons[k] = model.addConstr(expr == 1.0, namebuf.str().c_str());
                }

                std::vector<std::vector<std::vector<GRBConstr>>> design_cons(num_commodities, std::vector<std::vector<GRBConstr>>(num_nodes, std::vector<GRBConstr>(num_nodes)));

                for (auto k = 0; k < num_commodities; ++k)
                {
                        for (auto &arc : list_of_arcs)
                        {

                                namebuf.str("");
                                namebuf << "design_" << k << "_[" << arc.first << "," << arc.second << "]";

                                expr = 0;
                                auto idx_of_arc = &arc - &list_of_arcs[0];
                                expr += z_vars[idx_of_arc];

                                for (auto &pathvar : x_vars[k])
                                {
                                        auto path_idx = &pathvar - &x_vars[k][0];
                                        auto pth = rcspp_all_comms[k][path_idx];
                                        for (auto it = pth.begin(); it != pth.end() - 1; ++it)
                                        {
                                                if (arc.first == *it and arc.second == *(it + 1))
                                                {
                                                        expr -= x_vars[k][path_idx];
                                                }
                                        }
                                }

                                design_cons[k][arc.first][arc.second] = model.addConstr(expr >= 0.0, namebuf.str().c_str());
                        }
                }

                model.optimize();
        }
        catch (GRBException e)
        {
                std::cout << "Error code = " << e.getErrorCode() << std::endl;
                std::cout << e.getMessage() << std::endl;
        }
        catch (...)
        {
                std::cout << "Exception during optimization" << std::endl;
        }
}