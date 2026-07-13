// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/maximum_independent_set

#include <iostream>
#include <vector>
#include "../../src/algorithm/graph/maximum_independent_set.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    while(m--){
        int u, v;
        std::cin >> u >> v;
        graph[static_cast<std::size_t>(u)].push_back(v);
        graph[static_cast<std::size_t>(v)].push_back(u);
    }
    const auto answer = maximum_independent_set(graph);
    std::cout << answer.size() << '\n';
    for(std::size_t i = 0; i < answer.size(); ++i){
        if(i != 0) std::cout << ' ';
        std::cout << answer[i];
    }
    std::cout << '\n';
}
