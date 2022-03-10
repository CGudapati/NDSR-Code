//
//  NDSR_reader.hpp
//  NDSR_basemodel
//
//  Created by Naga V Gudapati on 12/17/19.
//  Copyright © 2019 Naga V Gudapati. All rights reserved.
//

#ifndef NDSR_Reader_h
#define NDSR_Reader_h

#include "NDSR_Instance.hpp"

#include "helper.h"

#include <fstream>
#include <sstream>

inline std::vector<std::string> split_NDSR(const std::string& s, char delimiter)
{
        std::vector<std::string> tokens;
        std::string              token;
        std::istringstream       tokenStream(s);
        while (std::getline(tokenStream, token, delimiter))
        {
                tokens.push_back(token);
        }
        return tokens;
}

inline NDSR_data read_instance_NDSR(const std::string filepath, double weight_multiplier)
{

        std::ifstream infile(filepath);
        int           N = 0;
        int           A = 0;
        int           K = 0;
        int           w = 0;

        std::string num_nodes_str("num_nodes");
        std::string num_arcs_str("num_arcs");
        std::string num_commodities_str("num_commodities");
        std::string num_weights_str("num_weights");
        std::string commodities_start_str("*Commodities: ");
        std::string arcs_start_str("*Arcs:");

        for (std::string line; getline(infile, line);)
        {
                if (line.find(num_nodes_str) == 0u)
                {
                        N = std::stoi((split_NDSR(line, ' ')).back());
                }

                if (line.find(num_arcs_str) == 0u)
                {
                        A = std::stoi((split_NDSR(line, ' ')).back());
                }

                if (line.find(num_commodities_str) == 0u)
                {
                        K = std::stoi((split_NDSR(line, ' ')).back());
                }

                if (line.find(num_weights_str) == 0u)
                {
                        w = std::stoi((split_NDSR(line, ' ')).back());
                }
        }

        NDSR_data instance(N, A, K, w);

        std::ifstream infile2(filepath);

        for (std::string line2; getline(infile2, line2);)
        {
                if (line2.find(commodities_start_str) == 0u)
                {
                        for (int i = 0; i < K; ++i)
                        {
                                std::string commodity_line = "";
                                std::getline(infile2, commodity_line);

                                auto commodity_s_t_str_tokens = split_NDSR(commodity_line, ' ');

                                auto commodity_index = std::stoi(commodity_s_t_str_tokens[0]) - 1; // Get the index of the commodity which is 0 based
                                auto source_node     = std::stoi(commodity_s_t_str_tokens[1]) - 1; // Get the source of the commodity
                                auto terminal_node   = std::stoi(commodity_s_t_str_tokens[2]) - 1; // Get the terminal of the commodity

                                instance.set_commodity_s_t_info(commodity_index, source_node, terminal_node);

                                auto weight1 = std::stoi(commodity_s_t_str_tokens[4]); // Get the weight1 of the commodity //We need better way to indx them (maybe a slice?
                                auto weight2 = std::stoi(commodity_s_t_str_tokens[5]); // Get the weight2 of the commodity

                                weight1 = static_cast<int>(weight1 * weight_multiplier);
                                weight2 = static_cast<int>(weight2 * weight_multiplier);
                                instance.set_commodity_weights_info(commodity_index, weight1, weight2);
                        }
                }

                if (line2.find(arcs_start_str) == 0u)
                {
                        for (int i = 0; i < A; ++i)
                        {
                                std::string arc_line = "";
                                std::getline(infile2, arc_line);

                                auto arc_line_tokens = split_NDSR(arc_line, ' ');

                                auto arc_tail          = std::stoi(arc_line_tokens[1]) - 1;
                                auto arc_head          = std::stoi(arc_line_tokens[2]) - 1;
                                auto arc_fixed_cost    = std::stoi(arc_line_tokens[3]);
                                auto arc_variable_cost = std::stod(arc_line_tokens[4]);

                                std::vector<int> arc_metrics;
                                arc_metrics.push_back(std::stoi(arc_line_tokens[5]));
                                arc_metrics.push_back(std::stoi(arc_line_tokens[6]));

                                instance.set_fixed_costs(arc_tail, arc_head, arc_fixed_cost);

                                for (auto k = 0; k < K; ++k)
                                { // For each commodity we send the variable cost
                                        instance.set_variable_costs(k, arc_tail, arc_head, arc_variable_cost);
                                }


                                for (auto j = 0; j < w; ++j)
                                {
                                        for (auto k = 0; k < K; ++k)
                                        { // For each commodity we send the variable cost
                                                instance.set_arc_metrics(j, k, arc_tail, arc_head, arc_metrics[j]);
                                        }
                                }
                        }
                }
        }

        auto list_of_arcs = instance.get_list_of_arcs();

        for (auto& arc : list_of_arcs)
        {
                int idx = static_cast<int>(&arc - &list_of_arcs[0]);
                instance.set_arc_index_dict(arc.first, arc.second, idx);
        }

        std::cout << "Problem Statistics:\n";
        std::cout << "Number of Nodes: " << instance.get_num_nodes() << "\n";
        std::cout << "Number of Arcs: " << instance.get_num_arcs() << "\n";
        std::cout << "Number of commodities: " << instance.get_num_commodities() << "\n";
        std::cout << "Number of weights: " << instance.get_num_metrics() << "\n";
        std::cout << "Finished Reading the Instance!\n";

        return instance;
}

#endif /* NDSR_reader_h */