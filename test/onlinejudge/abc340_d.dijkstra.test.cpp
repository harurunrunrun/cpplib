// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc340/tasks/abc340_d

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/dijkstra.hpp"

int main(){
    int n;
    std::cin >> n;
    std::vector<std::vector<DijkstraEdge<long long>>> graph(static_cast<std::size_t>(n));
    for(int v = 0; v + 1 < n; ++v){
        long long a, b;
        int x;
        std::cin >> a >> b >> x;
        graph[static_cast<std::size_t>(v)].push_back({v + 1, a});
        graph[static_cast<std::size_t>(v)].push_back({x - 1, b});
    }
    std::cout << dijkstra(graph, 0).dist[static_cast<std::size_t>(n - 1)] << '\n';
}
