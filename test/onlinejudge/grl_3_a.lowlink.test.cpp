// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/GRL_3_A

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/connectivity/lowlink.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int vertex_count, edge_count;
    std::cin >> vertex_count >> edge_count;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(edge_count));
    for(auto& [from, to]: edges) std::cin >> from >> to;
    for(int vertex: lowlink(vertex_count, edges).articulation){
        std::cout << vertex << '\n';
    }
}
