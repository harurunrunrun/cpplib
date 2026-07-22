// competitive-verifier: PROBLEM https://www.spoj.com/problems/CAPCITY/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/connectivity/unique_sink_scc_vertices.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int size, edge_count;
    std::cin >> size >> edge_count;
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(size));
    while(edge_count-- != 0){
        int from, to;
        std::cin >> from >> to;
        graph[static_cast<std::size_t>(from - 1)].push_back(to - 1);
    }
    const auto answer = unique_sink_scc_vertices(graph);
    std::cout << answer.size() << '\n';
    for(const int vertex: answer) std::cout << vertex + 1 << ' ';
    std::cout << '\n';
}
