// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/chromatic_number

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/constraints/chromatic_number.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    int edge_count;
    std::cin >> vertex_count >> edge_count;
    std::vector<std::vector<int>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    while(edge_count-- > 0){
        int first;
        int second;
        std::cin >> first >> second;
        graph[static_cast<std::size_t>(first)].push_back(second);
        graph[static_cast<std::size_t>(second)].push_back(first);
    }
    std::cout << chromatic_number(graph) << '\n';
}
