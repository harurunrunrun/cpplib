// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/minimum_diameter_spanning_tree

#include <iostream>
#include <vector>
#include "../../src/algorithm/graph/spanning_tree/minimum_diameter_spanning_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    std::vector<DiameterSpanningTreeEdge> edges(static_cast<std::size_t>(m));
    for(auto& edge: edges) std::cin >> edge.from >> edge.to >> edge.weight;
    const auto result = minimum_diameter_spanning_tree(n, edges);
    if(!result.exists) return 1;
    std::cout << result.diameter << '\n';
    for(int index: result.edge_indices) std::cout << index << ' ';
    std::cout << '\n';
}
