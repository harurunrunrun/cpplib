// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/cycle_detection

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
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(auto& [from, to]: edges){
        std::cin >> from >> to;
        graph[static_cast<std::size_t>(from)].push_back(to);
    }
    const auto cycle = directed_cycle(graph);
    if(cycle.empty()){
        std::cout << -1 << '\n';
        return 0;
    }
    std::vector<int> edge_ids;
    edge_ids.reserve(cycle.size());
    for(std::size_t i = 0; i < cycle.size(); ++i){
        const int from = cycle[i];
        const int to = cycle[(i + 1) % cycle.size()];
        for(int id = 0; id < m; ++id){
            if(edges[static_cast<std::size_t>(id)] == std::pair{from, to}){
                edge_ids.push_back(id);
                break;
            }
        }
    }
    std::cout << cycle.size() << '\n';
    for(int vertex: cycle) std::cout << vertex << '\n';
    for(int id: edge_ids) std::cout << id << '\n';
}
