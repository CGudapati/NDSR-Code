//
//  NDSR_Instance.cpp
//  NDSR_basemodel
//
//  Created by Naga V Gudapati on 12/17/19.
//  Copyright © 2019 Naga V Gudapati. All rights reserved.
//

#include "NDSR_Instance.hpp"


int NDSR_data::get_num_nodes(){
    return _num_nodes;
}


int NDSR_data::get_num_arcs() {
    return _num_arcs;
}

int NDSR_data::get_num_commodities() {
    return _num_commodities;
}




int NDSR_data::get_num_metrics(){
    return _num_metrics;
}


std::vector<std::vector<double>> NDSR_data::get_fixed_costs(){
    return _fixed_costs;
}

//std::unordered_map<vc_tuple_key, double, vc_KeyHash, vc_KeyEqual> &NDSR_data::get_variable_costs(){
//    return _variable_costs;
//}

std::vector<std::tuple<int, int, int>>  NDSR_data::get_variable_costs_indexes(){
    return _variable_costs_indexes;
}
std::vector<std::vector<int>>  NDSR_data::get_adj(){
    return _adj;
}

std::vector<std::pair<int, int>>  NDSR_data::get_commodity_s_t_info() {
    return _commodity_s_t_info;
}

std::vector<std::pair<int,int>>    NDSR_data::get_list_of_arcs() {
    return _list_of_arcs;
}




std::vector<std::vector<double>>                   NDSR_data::get_commodity_weights_info(){
    return _commodity_weights_info;
}


std::vector< std::vector<std::pair<int, double>>>  NDSR_data::get_commodity_graph(int commodity_index){
    return _graph[commodity_index];
}

std::vector<std::vector<std::vector<std::vector<double> > > > NDSR_data::get_arc_metrics_vec(){
    return _arc_metrics_vec;
}

std::vector<std::vector<std::vector<double>>> NDSR_data::get_variable_costs_vec() {
    return _variable_costs_vec;
}

void get_path(std::vector<int> prev, int j, std::vector<int>& path)
{
    
    if (j > static_cast<int>(prev.size())){
        throw  "No SPP";
    }
    if(prev[j] == -1)
        return;
    get_path(prev, prev[j], path);
    path.push_back(j);
}

double NDSR_data::get_path_cost(int commodity_index, std::vector<int>& path) {
//    assert(path.size() > 1);
    double path_cost = 0.0;
    if (path.size() > 1){
        for (auto it = path.begin(); it != path.end()-1; ++it) {
            path_cost += _variable_costs_vec[commodity_index][*it][*(it+1)];
        }
        
        return path_cost;
    }
    else{
        return std::numeric_limits<double>::max();
    }
}

double NDSR_data::get_path_cost_per_metric(int commodity_index, int metric_index, std::vector<int>& path)  //The cost is in terms of the metrics.
{
    double path_cost = 0.0;
    if (path.size() > 0)
    {
        for (auto it = path.begin(); it != path.end()-1; ++it)
        {
            path_cost += _arc_metrics_vec[metric_index][commodity_index][*it][*(it+1)];
        }
        return path_cost;
    }
    else
    {
        return std::numeric_limits<double>::max();
    }
}


double NDSR_data::get_path_cost(int commodity_index, std::vector<int>& path, std::vector<double>& reduced_costs){
//    assert(path.size() > 1);
    double path_cost = 0.0;
    if (path.size() > 1){
        for (auto it = path.begin(); it != path.end()-1; ++it) {
            //            path_cost += _variable_costs_vec[commodity_index][*it][*(it+1)] + reduced_costs[_arc_index_dict[std::make_pair(*it, *(it+1))]];
            path_cost += _variable_costs_vec[commodity_index][*it][*(it+1)] + reduced_costs[_arc_index_dict[*it][*(it+1)]];
        }
        
        return path_cost;
    }
    else{
        return std::numeric_limits<double>::max();
    }
}

std::vector<std::vector<int>>    NDSR_data::get_arc_idx_dict() {
    return _arc_index_dict;
}


std::vector<int> NDSR_data::get_spp(int commodity_index, int source, int sink) {
    
    
    std::vector<int> shortest_path;
    
    shortest_path.push_back(source);
    
    
    std::vector<int> prev(_num_nodes, std::numeric_limits<int>::max());
    
    prev[source] = -1;
    
    
    std::priority_queue< std::pair<int,int>, std::vector <std::pair<int,int>> , std::greater<std::pair<int,int>> > pq;
    

    std::vector<int> dist(_num_nodes, std::numeric_limits<int>::max());
    
    pq.push(std::make_pair(0, source));
    
    dist[source] = 0;
    

    while (!pq.empty())
    {
       
        int u = pq.top().second;
        pq.pop();
        
        for (auto x : _graph[commodity_index][u])
        {
            int v = x.first;
            double weight = x.second;

            if (dist[v] > dist[u] + weight)
            {
                dist[v] = dist[u] + weight;
                pq.push(std::make_pair(dist[v], v));
                prev[v] = u;
            }
        }
    }
    
    get_path(prev, sink, shortest_path);
    
    
    return shortest_path;
    
}



std::vector<int> NDSR_data::get_spp(int commodity_index, int source, int sink, std::vector<double>& reduced_costs){
    
    
    std::vector<int> shortest_path;
    
    shortest_path.push_back(source);
    
    
    std::vector<int> prev(_num_nodes, std::numeric_limits<int>::max());
    
    prev[source] = -1;
    
    
    std::priority_queue< std::pair<int,int>, std::vector <std::pair<int,int>> , std::greater<std::pair<int,int>> > pq;
    
    std::vector<int> dist(_num_nodes, std::numeric_limits<int>::max());
    
    pq.push(std::make_pair(0, source));
    
    dist[source] = 0;
    
    while (!pq.empty())
    {
        int u = pq.top().second;
        pq.pop();
        

        for (auto x : _graph[commodity_index][u])
        {

            int v = x.first;
            double weight = x.second + reduced_costs[_arc_index_dict[u][v]];

            if (dist[v] > dist[u] + weight)
            {

                dist[v] = dist[u] + weight;
                pq.push(std::make_pair(dist[v], v));
                prev[v] = u;
            }
        }
    }
    
    
    get_path(prev, sink, shortest_path);
    
    
    return shortest_path;
    
}



std::vector<int> NDSR_data::get_metric_spp(int commodity_index, int source, int sink, int metric_index)
{
    std::vector<int> shortest_path;
    
    shortest_path.push_back(source);
    
    std::vector<int> prev(_num_nodes, std::numeric_limits<int>::max());
    
    prev[source] = -1;
    
    
    std::priority_queue< std::pair<int,int>, std::vector <std::pair<int,int>> , std::greater<std::pair<int,int>> > pq;
    
    
    std::vector<int> dist(_num_nodes, std::numeric_limits<int>::max());
    
    
    pq.push(std::make_pair(0, source));
    
    dist[source] = 0;
    
    
    while (!pq.empty())
    {
    
        int u = pq.top().second;
        pq.pop();
        
    
        for (auto x : _graph[commodity_index][u])
        {
    
            
            int v = x.first;
            
    
            double weight = _arc_metrics_vec[metric_index][commodity_index][u][v];
            
            
    
            if (dist[v] > dist[u] + weight)
            {
    
                dist[v] = dist[u] + weight;
                pq.push(std::make_pair(dist[v], v));
                prev[v] = u;
            }
        }
    }
    
    get_path(prev, sink, shortest_path);
    
    
    return shortest_path;
}





bool NDSR_data::has_arc(int commodity_index, int u, int v){
    
    auto it = std::find_if( _graph[commodity_index][u].begin(), _graph[commodity_index][u].end(), [v](const std::pair <int, double>& element){ return element.first == v;} );
    if (it != _graph[commodity_index][u].end()){
        return true;
    }
    return false;
    
}

bool NDSR_data::does_path_satisfy_RCs(int commodity_index, std::vector<int>& path)
{
    double weight_of_path = 0.0;
    
    for (int m = 0; m < _num_metrics; ++m) {
        weight_of_path = 0.0;
        for (auto it = path.begin(); it != path.end()-1; ++it) {
            weight_of_path += _arc_metrics_vec[m][commodity_index][*it][*(it+1)];
        }
        if (weight_of_path > _commodity_weights_info[commodity_index][m] ){
            return false;
        }
    }
    return true;
}

bool NDSR_data::does_path_satisfy_RCs_for_a_metric(int commodity_index,int metric_index, std::vector<int>& path)
{
        
        double weight_of_path = 0.0;
    
        for (auto it = path.begin(); it != path.end()-1; ++it) {
            weight_of_path += _arc_metrics_vec[metric_index][commodity_index][*it][*(it+1)];
        }
        if (weight_of_path > _commodity_weights_info[commodity_index][metric_index] )
        {
            return false;
        }
   
    return true;
}


void NDSR_data::delete_arc (int commodity_index, int u, int v)      //Only has to modify the _graph variable
{
    auto it = std::find_if( _graph[commodity_index][u].begin(), _graph[commodity_index][u].end(), [v]( std::pair <int, double>& element){ return element.first == v;} );
    if (it != _graph[commodity_index][u].end()){
        _graph[commodity_index][u].erase(it);
    }
}

double NDSR_data::get_arc_costs(int commodity_index, int u, int v)
{
    return _variable_costs_vec[commodity_index][u][v];
}

void NDSR_data::add_arc(int commodity_index, int u, int v, int weight){
    _graph[commodity_index][u].push_back(std::make_pair(v, weight));
}



void NDSR_data::set_commodity_s_t_info(int commodity_index, int source, int terminal){
    _commodity_s_t_info[commodity_index] = std::make_pair(source,terminal);
}


void NDSR_data::set_commodity_weights_info(int commodity_index, double weight1, double weight2){
    std::vector<double> weight_vector;
    weight_vector.push_back(weight1);
    weight_vector.push_back(weight2);
    _commodity_weights_info[commodity_index] = weight_vector;
}

void NDSR_data::set_fixed_costs(int arc_tail, int arc_head, int fixed_cost){
    _fixed_costs[arc_tail][arc_head] = fixed_cost;
    _adj[arc_tail].push_back(arc_head);
    _list_of_arcs.push_back(std::make_pair(arc_tail, arc_head));
}

void NDSR_data::set_variable_costs(int commodity_index, int arc_tail, int arc_head, double variable_cost ){
    _graph[commodity_index][arc_tail].push_back(std::make_pair(arc_head, variable_cost));    
    _variable_costs_vec[commodity_index][arc_tail][arc_head]=variable_cost;
}


void NDSR_data::set_arc_metrics(int metric_index, int commodity_index, int arc_tail, int arc_head, double arc_metric_weight){
    _arc_metrics_vec[metric_index][commodity_index][arc_tail][arc_head] = arc_metric_weight;
}

void NDSR_data::set_arc_index_dict (int u, int v, int idx){
 
    _arc_index_dict[u][v] = idx;
}