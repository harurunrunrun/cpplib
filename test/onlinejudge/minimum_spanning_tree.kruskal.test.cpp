// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/minimum_spanning_tree

#include <iostream>
#include <vector>
#include "../../src/algorithm/graph/kruskal_mst.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    std::vector<KruskalEdge<long long>> edges(static_cast<std::size_t>(m));
    for(auto& edge: edges) std::cin >> edge.from >> edge.to >> edge.cost;
    const auto result = kruskal_mst(n, edges);
    std::cout << result.cost << '\n';
    for(std::size_t i = 0; i < result.edge_ids.size(); ++i){
        if(i != 0) std::cout << ' ';
        std::cout << result.edge_ids[i];
    }
    std::cout << '\n';
}
