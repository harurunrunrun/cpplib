// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc002/tasks/abc002_4

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/maximum_clique.hpp"

int main(){
    int n, m;
    std::cin >> n >> m;
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(int i = 0; i < m; ++i){
        int u, v;
        std::cin >> u >> v;
        --u;
        --v;
        graph[static_cast<std::size_t>(u)].push_back(v);
        graph[static_cast<std::size_t>(v)].push_back(u);
    }
    std::cout << maximum_clique(graph).size() << '\n';
}
