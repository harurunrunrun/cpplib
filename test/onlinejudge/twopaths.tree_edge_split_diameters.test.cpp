// competitive-verifier: PROBLEM https://www.spoj.com/problems/TWOPATHS/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/tree_edge_split_diameters.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    std::cin >> vertex_count;
    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<std::size_t>(vertex_count - 1));
    for(int edge = 0; edge + 1 < vertex_count; ++edge){
        int left, right;
        std::cin >> left >> right;
        edges.emplace_back(left - 1, right - 1);
    }
    const auto answer =
        maximum_product_of_two_vertex_disjoint_tree_paths<100000>(vertex_count, edges);
    std::cout << answer.product << '\n';
}
