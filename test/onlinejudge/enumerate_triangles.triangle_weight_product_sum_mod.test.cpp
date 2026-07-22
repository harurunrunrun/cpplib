// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/enumerate_triangles

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/specialized/triangle_weight_product_sum_mod.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr long long MODULUS = 998244353;
    int vertex_count, edge_count;
    std::cin >> vertex_count >> edge_count;
    std::vector<long long> weights(static_cast<std::size_t>(vertex_count));
    for(long long& weight: weights) std::cin >> weight;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(edge_count));
    for(auto& [left, right]: edges) std::cin >> left >> right;

    std::cout << triangle_weight_product_sum_mod(
        vertex_count, edges, weights, MODULUS
    ) << '\n';
}
