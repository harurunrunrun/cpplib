// competitive-verifier: PROBLEM https://www.spoj.com/problems/MST/

#include <iostream>
#include <tuple>
#include <vector>

#include "../../src/algorithm/graph/spanning_tree/minimum_spanning_tree_total_weight.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count;
    if(!(std::cin >> vertex_count >> edge_count)) return 0;
    std::vector<std::tuple<int, int, long long>> edges;
    edges.reserve(static_cast<std::size_t>(edge_count));
    for(int index = 0; index < edge_count; index++){
        int left, right;
        long long weight;
        std::cin >> left >> right >> weight;
        edges.emplace_back(left - 1, right - 1, weight);
    }
    std::cout << minimum_spanning_tree_total_weight(vertex_count, edges) << '\n';
}
