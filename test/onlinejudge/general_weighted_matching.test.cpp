// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/general_weighted_matching

#include <iostream>
#include <vector>
#include "../../src/algorithm/matching/general/maximum_weight_general_matching.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    std::vector<MaximumWeightGeneralMatchingEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(m));
    while(m--){
        int from, to;
        long long weight;
        std::cin >> from >> to >> weight;
        edges.push_back({from, to, weight});
    }
    const auto result = maximum_weight_general_matching(n, edges);
    std::cout << result.size << ' ' << result.weight << '\n';
    for(int vertex = 0; vertex < n; ++vertex){
        const int other = result.match[static_cast<std::size_t>(vertex)];
        if(vertex < other) std::cout << vertex << ' ' << other << '\n';
    }
}
