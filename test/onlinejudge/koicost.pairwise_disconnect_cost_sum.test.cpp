// competitive-verifier: PROBLEM https://www.spoj.com/problems/KOICOST/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/pairwise_disconnect_cost_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;
    std::vector<PairwiseDisconnectCostEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int edge_index = 0; edge_index < m; edge_index++){
        int from, to;
        long long weight;
        std::cin >> from >> to >> weight;
        edges.push_back({from - 1, to - 1, weight});
    }
    constexpr long long MODULUS = 1'000'000'000LL;
    std::cout << pairwise_disconnect_cost_sum(n, edges, MODULUS) << '\n';
}
