// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/cycle_detection_undirected

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/undirected_cycle_certificate.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count;
    std::cin >> vertex_count >> edge_count;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(edge_count));
    for(auto& [left, right]: edges) std::cin >> left >> right;

    const auto cycle = undirected_cycle_certificate(vertex_count, edges);
    if(cycle.vertices.empty()){
        std::cout << -1 << '\n';
        return 0;
    }
    std::cout << cycle.vertices.size() << '\n';
    for(int vertex: cycle.vertices) std::cout << vertex << '\n';
    for(int edge: cycle.edges) std::cout << edge << '\n';
}
