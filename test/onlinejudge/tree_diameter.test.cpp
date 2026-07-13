// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/tree_diameter

#include <iostream>
#include <utility>
#include <vector>
#include "../../src/algorithm/tree/tree_diameter.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    std::vector<std::vector<std::pair<int, long long>>> graph(static_cast<std::size_t>(n));
    for(int i = 1; i < n; ++i){
        int u, v;
        long long weight;
        std::cin >> u >> v >> weight;
        graph[static_cast<std::size_t>(u)].push_back({v, weight});
        graph[static_cast<std::size_t>(v)].push_back({u, weight});
    }
    const auto result = weighted_tree_diameter(graph);
    std::cout << result.length << ' ' << result.path.size() << '\n';
    for(std::size_t i = 0; i < result.path.size(); ++i){
        if(i != 0) std::cout << ' ';
        std::cout << result.path[i];
    }
    std::cout << '\n';
}
