// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/general_weighted_matching

#include <iostream>
#include <vector>
#include "../../src/algorithm/matching/minimum_weight_general_matching.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    std::vector<MinimumWeightGeneralMatchingEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(m));
    while(m--){
        int u, v;
        long long weight;
        std::cin >> u >> v >> weight;
        edges.push_back({u, v, -weight});
    }
    const auto result = minimum_weight_general_matching(n, edges);
    std::cout << result.size << ' ' << -result.cost << '\n';
    for(int v = 0; v < n; ++v){
        const int to = result.match[static_cast<std::size_t>(v)];
        if(v < to) std::cout << v << ' ' << to << '\n';
    }
}
