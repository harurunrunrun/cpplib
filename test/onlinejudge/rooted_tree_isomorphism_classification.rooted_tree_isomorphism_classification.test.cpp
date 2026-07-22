// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/rooted_tree_isomorphism_classification

#include <iostream>
#include <vector>
#include "../../src/algorithm/tree/isomorphism/rooted_tree_isomorphism_classification.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(int vertex = 1; vertex < n; ++vertex){
        int parent;
        std::cin >> parent;
        graph[static_cast<std::size_t>(parent)].push_back(vertex);
        graph[static_cast<std::size_t>(vertex)].push_back(parent);
    }
    const auto result = rooted_tree_isomorphism_classification(graph);
    std::cout << result.class_count << '\n';
    for(int vertex = 0; vertex < n; ++vertex){
        if(vertex != 0) std::cout << ' ';
        std::cout << result.class_id[static_cast<std::size_t>(vertex)];
    }
    std::cout << '\n';
}
