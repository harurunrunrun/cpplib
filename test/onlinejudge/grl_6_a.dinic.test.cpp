// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/GRL_6_A

#include <iostream>

#include "../../src/algorithm/matching/dinic.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int vertex_count, edge_count;
    std::cin >> vertex_count >> edge_count;
    Dinic<long long> flow(vertex_count);
    while(edge_count--){
        int from, to;
        long long capacity;
        std::cin >> from >> to >> capacity;
        flow.add_edge(from, to, capacity);
    }
    std::cout << flow.max_flow(0, vertex_count - 1) << '\n';
}
