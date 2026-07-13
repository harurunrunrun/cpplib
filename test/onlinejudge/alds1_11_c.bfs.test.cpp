// competitive-verifier: PROBLEM https://judge.u-aizu.ac.jp/onlinejudge/description.jsp?id=ALDS1_11_C

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/bfs.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(int i = 0; i < n; ++i){
        int u, degree;
        std::cin >> u >> degree;
        --u;
        auto& edges = graph[static_cast<std::size_t>(u)];
        edges.resize(static_cast<std::size_t>(degree));
        for(int& to: edges){
            std::cin >> to;
            --to;
        }
    }
    auto result = bfs(graph, 0);
    for(int v = 0; v < n; ++v){
        std::cout << v + 1 << ' ' << result.dist[static_cast<std::size_t>(v)] << '\n';
    }
}
