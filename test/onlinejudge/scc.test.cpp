// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/scc

#include <iostream>
#include <vector>
#include "../../src/algorithm/graph/strongly_connected_components.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    while(m--){
        int from, to;
        std::cin >> from >> to;
        graph[static_cast<std::size_t>(from)].push_back(to);
    }
    const auto result = strongly_connected_components(graph);
    std::cout << result.count << '\n';
    for(const auto& group: result.groups){
        std::cout << group.size();
        for(int vertex: group) std::cout << ' ' << vertex;
        std::cout << '\n';
    }
}
