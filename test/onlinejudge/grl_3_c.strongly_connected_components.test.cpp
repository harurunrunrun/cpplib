#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/GRL_3_C"

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/strongly_connected_components.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int vertex_count, edge_count;
    std::cin >> vertex_count >> edge_count;
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(vertex_count));
    while(edge_count--){
        int from, to;
        std::cin >> from >> to;
        graph[static_cast<std::size_t>(from)].push_back(to);
    }
    const auto result = strongly_connected_components(graph);
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        int from, to;
        std::cin >> from >> to;
        std::cout << (result.id[static_cast<std::size_t>(from)] ==
                      result.id[static_cast<std::size_t>(to)]) << '\n';
    }
}
