// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/cycle_detection_undirected

#include <iostream>
#include <utility>
#include <vector>
#include "../../src/algorithm/graph/cycle_detection.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [u, v]: edges) std::cin >> u >> v;
    const auto cycle = undirected_cycle(n, edges);
    if(cycle.empty()){
        std::cout << -1 << '\n';
        return 0;
    }
    std::vector<char> used(static_cast<std::size_t>(m), 0);
    std::vector<int> edge_ids;
    edge_ids.reserve(cycle.size());
    for(std::size_t i = 0; i < cycle.size(); ++i){
        const int u = cycle[i];
        const int v = cycle[(i + 1) % cycle.size()];
        for(int id = 0; id < m; ++id){
            const auto [a, b] = edges[static_cast<std::size_t>(id)];
            if(!used[static_cast<std::size_t>(id)] &&
               ((a == u && b == v) || (a == v && b == u))){
                used[static_cast<std::size_t>(id)] = 1;
                edge_ids.push_back(id);
                break;
            }
        }
    }
    std::cout << cycle.size() << '\n';
    for(int vertex: cycle) std::cout << vertex << '\n';
    for(int id: edge_ids) std::cout << id << '\n';
}
