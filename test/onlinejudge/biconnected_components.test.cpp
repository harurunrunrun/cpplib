// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/biconnected_components

#include <iostream>
#include <utility>
#include <vector>
#include "../../src/algorithm/graph/two_vertex_connected_components.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [u, v]: edges) std::cin >> u >> v;
    const auto result = two_vertex_connected_components(n, edges);
    std::cout << result.count << '\n';
    for(const auto& group: result.vertex_groups){
        std::cout << group.size();
        for(int vertex: group) std::cout << ' ' << vertex;
        std::cout << '\n';
    }
}
