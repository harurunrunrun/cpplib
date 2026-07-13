// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/k_shortest_walk

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/kth_shortest_walk.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, edge_count, source, target, required;
    std::cin >> vertex_count >> edge_count >> source >> target >> required;
    std::vector<std::vector<KthShortestWalkEdge<long long>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    while(edge_count--){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        graph[static_cast<std::size_t>(from)].push_back({to, cost});
    }
    const auto result = kth_shortest_walks(
        graph, source, target, required
    );
    for(long long distance: result) std::cout << distance << '\n';
    for(int index = static_cast<int>(result.size()); index < required; ++index){
        std::cout << -1 << '\n';
    }
}
