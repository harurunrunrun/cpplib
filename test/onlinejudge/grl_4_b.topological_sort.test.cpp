#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/GRL_4_B"

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/topological_sort.hpp"

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
    for(int vertex: topological_sort(graph).order) std::cout << vertex << '\n';
}
