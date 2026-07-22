// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/specialized/triangle_weight_product_sum_mod.hpp"

namespace{

void self_test(){
    assert(triangle_weight_product_sum_mod(
        4,
        {{0, 1}, {1, 2}, {2, 0}, {0, 3}},
        {2, 3, 5, 100},
        998244353
    ) == 30);
    assert(triangle_weight_product_sum_mod(
        3, {{0, 1}, {1, 2}, {2, 0}}, {-1, 2, 3}, 7
    ) == 1);

    bool thrown = false;
    try{
        (void)triangle_weight_product_sum_mod(2, {{0, 1}}, {1}, 7);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

} // namespace

int main(){
    self_test();

    constexpr long long MODULUS = 998244353;
    int vertex_count, edge_count;
    if(!(std::cin >> vertex_count >> edge_count)) return 0;
    std::vector<long long> weights(static_cast<std::size_t>(vertex_count));
    for(long long& weight: weights) std::cin >> weight;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(edge_count));
    for(auto& [left, right]: edges) std::cin >> left >> right;
    std::cout << triangle_weight_product_sum_mod(
        vertex_count, edges, weights, MODULUS
    ) << '\n';
}
