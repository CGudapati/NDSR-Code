//
//  NDSR_Instance.hpp
//  NDSR_basemodel
//
//  Created by Naga V Gudapati on 12/17/19.
//  Copyright © 2019 Naga V Gudapati. All rights reserved.
//

#ifndef NDSR_Instance_hpp
#define NDSR_Instance_hpp

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "helper.h"

class NDSR_data
{
private:
    int _num_nodes;
    int _num_arcs;
    int _num_commodities;
    int _num_metrics;
    std::vector<std::pair<int, int>> _commodity_s_t_info;
    std::vector<std::vector<double>> _commodity_weights_info;

    std::vector<std::vector<double>> _fixed_costs;
    std::vector<std::vector<int>> _adj;
    std::vector<std::pair<int, int>> _list_of_arcs; // List of all arcs and they will be added when reading the fixed costs.
    std::vector<std::vector<int>> _arc_index_dict;

    std::vector<std::tuple<int, int, int>> _variable_costs_indexes;
    std::vector<std::vector<std::vector<std::pair<int, double>>>> _graph;
    std::vector<std::vector<std::vector<std::vector<double>>>> _arc_metrics_vec;
    std::vector<std::vector<std::vector<double>>> _variable_costs_vec;

public:
    int get_num_nodes();
    int get_num_arcs();
    int get_num_commodities();
    int get_num_metrics();
    double get_path_cost(int commodity_index, std::vector<int> &path);
    double get_path_cost(int commodity_index, std::vector<int> &path, std::vector<double> &reduced_costs);
    double get_path_cost_per_metric(int commodity_index, int metric_index, std::vector<int> &path);
    double get_arc_costs(int commodity_index, int u, int v);

    bool has_arc(int commodity_index, int u, int v);
    bool does_path_satisfy_RCs(int commodity_index, std::vector<int> &path);
    bool does_path_satisfy_RCs_for_a_metric(int commodity_index, int metric_index, std::vector<int> &path);

    std::vector<std::vector<double>> get_fixed_costs();

    std::vector<std::vector<int>> get_adj();
    std::vector<std::pair<int, int>> get_commodity_s_t_info();
    std::vector<std::pair<int, int>> get_list_of_arcs();
    std::vector<std::vector<double>> get_commodity_weights_info();
    std::vector<std::vector<std::pair<int, double>>> get_commodity_graph(int commodity_index);
    std::vector<int> get_spp(int commdoity_index, int source, int sink);
    std::vector<int> get_metric_spp(int commodity_index, int source, int sink, int metric_index);
    std::vector<int> get_spp(int commdoity_index, int source, int sink, std::vector<double> &reduced_costs);
    std::vector<std::vector<int>> get_arc_idx_dict();
    std::vector<std::vector<std::vector<std::vector<double>>>> get_arc_metrics_vec(); // This is better!
    std::vector<std::vector<std::vector<double>>> get_variable_costs_vec();           // This is better!
    std::vector<std::tuple<int, int, int>> get_variable_costs_indexes();

    void set_commodity_s_t_info(int commodity_index, int source, int terminal);
    void set_commodity_weights_info(int commodity_index, double weight1, double weight2);
    void set_fixed_costs(int arc_tail, int arc_head, int fixed_cost); // Will also be used to set _list_of_arcs and adj
    void set_variable_costs(int commodity_index, int arc_tail, int arc_head, double variable_cost);
    void set_arc_metrics(int metric_index, int commodity_index, int arc_tail, int arc_head, double arc_metric_weight);
    void set_arc_index_dict(int u, int v, int idx);
    void delete_arc(int commodity_index, int u, int v); // Only has to modify the _graph variable
    void add_arc(int commodity_index, int u, int v, int weight);

    // constructor to create the NDSR instance. This is just a simple class to hold some data together.
    NDSR_data(int N, int A, int K, int w)
    {
        _num_nodes = N;
        _num_arcs = A;
        _num_commodities = K;
        _num_metrics = w;

        _adj.resize(N);
        _graph = std::vector<std::vector<std::vector<std::pair<int, double>>>>(K, std::vector<std::vector<std::pair<int, double>>>(N));
        _arc_metrics_vec = std::vector<std::vector<std::vector<std::vector<double>>>>(w, std::vector<std::vector<std::vector<double>>>(K, std::vector<std::vector<double>>(N, std::vector<double>(N))));
        _variable_costs_vec = std::vector<std::vector<std::vector<double>>>(K, std::vector<std::vector<double>>(N, std::vector<double>(N)));
        _commodity_s_t_info = std::vector<std::pair<int, int>>(K);
        _commodity_weights_info = std::vector<std::vector<double>>(K);
        _fixed_costs = std::vector<std::vector<double>>(N, std::vector<double>(N));
        _arc_index_dict = std::vector<std::vector<int>>(N, std::vector<int>(N));
    };
};

#endif /* NDSR_Instance_hpp */
