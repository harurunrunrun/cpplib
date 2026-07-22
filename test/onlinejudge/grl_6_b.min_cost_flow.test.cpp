// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/GRL_6_B

#include <iostream>

#include "../../src/algorithm/matching/flow/min_cost_flow.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int vertex_count, edge_count;
    long long required_flow;
    std::cin >> vertex_count >> edge_count >> required_flow;
    MinCostFlow<long long> flow(vertex_count);
    while(edge_count--){
        int from, to;
        long long capacity, cost;
        std::cin >> from >> to >> capacity >> cost;
        flow.add_edge(from, to, capacity, cost);
    }
    const auto cost = flow.min_cost_for_exact_flow(0, vertex_count - 1, required_flow);
    std::cout << (cost ? *cost : -1) << '\n';
}
