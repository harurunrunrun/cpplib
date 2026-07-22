// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/dominatortree

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/connectivity/dominator_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, edge_count, source;
    std::cin >> n >> edge_count >> source;
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    while(edge_count-- > 0){
        int from, to;
        std::cin >> from >> to;
        graph[static_cast<std::size_t>(from)].push_back(to);
    }
    const auto result = dominator_tree(graph, source);
    for(int vertex = 0; vertex < n; ++vertex){
        if(vertex != 0) std::cout << ' ';
        std::cout << result.immediate_dominator[static_cast<std::size_t>(vertex)];
    }
    std::cout << '\n';
}
