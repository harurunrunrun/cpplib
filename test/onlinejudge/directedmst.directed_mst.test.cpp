// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/directedmst

#include <iostream>
#include <vector>
#include "../../src/algorithm/graph/spanning_tree/directed_mst.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m, root;
    std::cin >> n >> m >> root;
    std::vector<DirectedMstEdge<long long>> edges(static_cast<std::size_t>(m));
    for(auto& edge: edges) std::cin >> edge.from >> edge.to >> edge.cost;
    const auto result = directed_mst(n, root, edges);
    if(!result.exists) return 1;
    std::cout << result.cost << '\n';
    for(int vertex = 0; vertex < n; ++vertex){
        std::cout << result.parent[vertex] << (vertex + 1 == n ? '\n' : ' ');
    }
}
