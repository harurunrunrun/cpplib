// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/shortest_path

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include "../../src/algorithm/graph/dijkstra.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m, source, target;
    std::cin >> n >> m >> source >> target;
    std::vector<std::vector<DijkstraEdge<long long>>> graph(static_cast<std::size_t>(n));
    while(m--){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        graph[static_cast<std::size_t>(from)].push_back({to, cost});
    }
    const auto result = dijkstra(graph, source);
    if(!result.reachable[static_cast<std::size_t>(target)]){
        std::cout << -1 << '\n';
        return 0;
    }
    std::vector<std::pair<int, int>> path;
    for(int vertex = target; vertex != source; ){
        const int parent = result.parent[static_cast<std::size_t>(vertex)];
        path.push_back({parent, vertex});
        vertex = parent;
    }
    std::reverse(path.begin(), path.end());
    std::cout << result.dist[static_cast<std::size_t>(target)] << ' ' << path.size() << '\n';
    for(auto [from, to]: path) std::cout << from << ' ' << to << '\n';
}
