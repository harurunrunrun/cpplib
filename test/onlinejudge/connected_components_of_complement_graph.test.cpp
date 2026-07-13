// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/connected_components_of_complement_graph

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/connected_components_of_complement_graph.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, edge_count;
    std::cin >> n >> edge_count;
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    while(edge_count-- > 0){
        int left, right;
        std::cin >> left >> right;
        graph[static_cast<std::size_t>(left)].push_back(right);
        graph[static_cast<std::size_t>(right)].push_back(left);
    }
    const auto result = connected_components_of_complement_graph(graph);
    std::cout << result.groups.size() << '\n';
    for(const auto& group: result.groups){
        std::cout << group.size();
        for(int vertex: group) std::cout << ' ' << vertex;
        std::cout << '\n';
    }
}
