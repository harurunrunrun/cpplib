// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/global_minimum_cut_of_dynamic_star_augmented_graph

#include <iostream>
#include <vector>
#include "../../src/algorithm/graph/flow_cut/star_augmented_minimum_cut.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m, q;
    std::cin >> n >> m >> q;
    std::vector<long long> weights(static_cast<std::size_t>(n));
    for(auto& weight: weights) std::cin >> weight;
    std::vector<StarAugmentedCutEdge> edges(static_cast<std::size_t>(m));
    for(auto& edge: edges) std::cin >> edge.from >> edge.to >> edge.capacity;
    StarAugmentedMinimumCut graph(n, edges, weights);
    while(q--){
        int vertex;
        long long capacity;
        std::cin >> vertex >> capacity;
        std::cout << graph.set_weight(vertex, capacity) << '\n';
    }
}
