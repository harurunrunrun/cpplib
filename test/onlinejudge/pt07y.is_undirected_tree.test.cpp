// competitive-verifier: PROBLEM https://www.spoj.com/problems/PT07Y/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/traversal/is_undirected_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count;
    if(!(std::cin >> vertex_count >> edge_count)) return 0;
    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<std::size_t>(edge_count));
    for(int index = 0; index < edge_count; index++){
        int left, right;
        std::cin >> left >> right;
        edges.emplace_back(left - 1, right - 1);
    }
    std::cout << (is_undirected_tree(vertex_count, edges) ? "YES\n" : "NO\n");
}
