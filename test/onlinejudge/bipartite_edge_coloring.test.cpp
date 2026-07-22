// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/bipartite_edge_coloring

#include <iostream>
#include <vector>
#include "../../src/algorithm/graph/constraints/bipartite_edge_coloring.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int left, right, m;
    std::cin >> left >> right >> m;
    std::vector<BipartiteEdgeColoringEdge> edges(static_cast<std::size_t>(m));
    for(auto& edge: edges) std::cin >> edge.left >> edge.right;
    const auto result = bipartite_edge_coloring(left, right, edges);
    std::cout << result.color_count << '\n';
    for(int color: result.color) std::cout << color << '\n';
}
