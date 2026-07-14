// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/pairwise_disconnect_cost_sum.hpp"

using Edge = PairwiseDisconnectCostEdge<long long>;

void self_test(){
    constexpr long long MODULUS = 1'000'000'000LL;
    assert(pairwise_disconnect_cost_sum<long long>(1, {}, MODULUS) == 0);
    assert(pairwise_disconnect_cost_sum<long long>(2, {{0, 1, 5}}, MODULUS) == 5);
    assert(pairwise_disconnect_cost_sum<long long>(3, {
        {0, 1, 1}, {1, 2, 2}
    }, MODULUS) == 5);
    assert(pairwise_disconnect_cost_sum<long long>(3, {
        {0, 1, 1}, {1, 2, 2}, {0, 2, 3}
    }, MODULUS) == 12);
    assert(pairwise_disconnect_cost_sum<long long>(4, {
        {0, 1, 2}, {2, 3, 5}
    }, MODULUS) == 9);
    assert(pairwise_disconnect_cost_sum<long long>(2, {
        {0, 0, 1}, {0, 1, 2}
    }, MODULUS) == 3);
    assert(pairwise_disconnect_cost_sum<long long>(4, {
        {0, 1, 2}, {2, 3, 5}
    }, 5) == 4);

    {
        std::vector<Edge> sample = {
            {0, 1, 10}, {1, 2, 2}, {3, 2, 5}, {5, 2, 15},
            {2, 4, 4}, {3, 4, 3}, {1, 5, 6},
        };
        assert(pairwise_disconnect_cost_sum(6, sample, MODULUS) == 256);
    }
    {
        bool thrown = false;
        try{
            (void)pairwise_disconnect_cost_sum<long long>(2, {
                {0, 1, 3}, {0, 1, 3}
            }, MODULUS);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    {
        bool thrown = false;
        try{
            (void)pairwise_disconnect_cost_sum<long long>(2, {
                {0, 1, -1}
            }, MODULUS);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    {
        bool thrown = false;
        try{
            (void)pairwise_disconnect_cost_sum<long long>(2, {
                {0, 2, 1}
            }, MODULUS);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
    {
        bool thrown = false;
        try{
            (void)pairwise_disconnect_cost_sum<long long>(2, {}, 0);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
}

int main(){
    self_test();

    int n, m;
    long long modulus;
    if(!(std::cin >> n >> m >> modulus)) return 0;
    std::vector<Edge> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int edge_index = 0; edge_index < m; edge_index++){
        int from, to;
        long long weight;
        std::cin >> from >> to >> weight;
        edges.push_back({from, to, weight});
    }
    std::cout << pairwise_disconnect_cost_sum(n, edges, modulus) << '\n';
}
