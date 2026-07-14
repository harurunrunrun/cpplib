// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/shortest_path

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/directed_shortest_path_route.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count, source, target;
    std::cin >> vertex_count >> edge_count >> source >> target;
    std::vector<DirectedShortestPathRouteEdge<long long>> edges;
    edges.reserve(static_cast<std::size_t>(edge_count));
    while(edge_count-- > 0){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        edges.push_back({from, to, cost});
    }

    const auto answer = directed_shortest_path_route(
        vertex_count, edges, source, target
    );
    if(!answer.reachable){
        std::cout << -1 << '\n';
        return 0;
    }
    std::cout << answer.distance << ' ' << answer.edges.size() << '\n';
    for(const auto& [from, to]: answer.edges){
        std::cout << from << ' ' << to << '\n';
    }
}
