// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/frequency_table_of_tree_distance

#include <iostream>
#include <utility>
#include <vector>
#include "../../src/algorithm/tree/dynamic_programming/tree_distance_histogram.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::cin >> n;
    std::vector<std::pair<int, int>> edges(n - 1); for(auto& [u, v]: edges) std::cin >> u >> v;
    auto answer = tree_distance_histogram(n, edges);
    for(int i = 1; i < n; ++i) std::cout << answer[i] << (i + 1 == n ? '\n' : ' ');
    if(n == 1) std::cout << '\n';
}
