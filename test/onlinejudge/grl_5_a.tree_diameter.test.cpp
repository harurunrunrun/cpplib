// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/GRL_5_A

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/tree_diameter.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int vertex_count;
    std::cin >> vertex_count;
    std::vector<std::vector<std::pair<int, long long>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(int i = 1; i < vertex_count; i++){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        graph[static_cast<std::size_t>(from)].push_back({to, cost});
        graph[static_cast<std::size_t>(to)].push_back({from, cost});
    }
    std::cout << weighted_tree_diameter(graph).length << '\n';
}
