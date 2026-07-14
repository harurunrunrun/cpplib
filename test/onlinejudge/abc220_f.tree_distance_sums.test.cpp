// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc220/tasks/abc220_f

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/tree_distance_sums.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<std::size_t>(n - 1));
    for(int i = 1; i < n; ++i){
        int u, v;
        std::cin >> u >> v;
        edges.emplace_back(u - 1, v - 1);
    }
    for(long long distance_sum: tree_distance_sums(n, edges)){
        std::cout << distance_sum << '\n';
    }
}
