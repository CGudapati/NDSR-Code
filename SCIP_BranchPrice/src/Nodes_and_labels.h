#ifndef NODES_AND_LABELS_H
#define NODES_AND_LABELS_H

#include <stack>
#include <vector>

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

inline bool is_v_in_l(int v, Label* lab, int s)
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

inline bool dominated(int u, Node* v, Label* lab, const std::vector<std::vector<double>>& var_costs_adj, const std::vector<std::vector<double>>& w1_adj, const std::vector<std::vector<double>>& w2_adj)
{
        for (int i = 0; i < v->m_num_labels; ++i)
        {
                auto v_label = v->m_labels[i];

                if (lab->m_cost + var_costs_adj[u][v->m_node_idx] >= v_label->m_cost and lab->m_m1_cons + w1_adj[u][v->m_node_idx] >= v_label->m_m1_cons and lab->m_m2_cons + w2_adj[u][v->m_node_idx] >= v_label->m_m2_cons)
                {
                        return true;
                }
                if (v_label->m_flag == false)
                {
                        continue;
                }
                if (lab->m_cost + var_costs_adj[u][v->m_node_idx] <= v_label->m_cost and lab->m_m1_cons + w1_adj[u][v->m_node_idx] <= v_label->m_m1_cons and lab->m_m2_cons + w2_adj[u][v->m_node_idx] <= v_label->m_m2_cons)
                {
                        v_label->m_flag = false;
                }
        }
        return false;
}

inline bool dominated_pricer(int k, int u, Node* v, Label* lab, const std::vector<std::vector<std::vector<double>>>& var_costs_per_all_k, const std::vector<std::vector<double>>& w1_adj, const std::vector<std::vector<double>>& w2_adj)
{
        for (int i = 0; i < v->m_num_labels; ++i)
        {
                auto v_label = v->m_labels[i];

                if (lab->m_cost + var_costs_per_all_k[k][u][v->m_node_idx] >= v_label->m_cost and lab->m_m1_cons + w1_adj[u][v->m_node_idx] >= v_label->m_m1_cons and lab->m_m2_cons + w2_adj[u][v->m_node_idx] >= v_label->m_m2_cons)
                {
                        return true;
                }
                if (v_label->m_flag == false)
                {
                        continue;
                }
                if (lab->m_cost + var_costs_per_all_k[k][u][v->m_node_idx] <= v_label->m_cost and lab->m_m1_cons + w1_adj[u][v->m_node_idx] <= v_label->m_m1_cons and lab->m_m2_cons + w2_adj[u][v->m_node_idx] <= v_label->m_m2_cons)
                {
                        v_label->m_flag = false;
                }
        }
        return false;
}

#endif /* NODES_AND_LABELS_H */
