// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc319/tasks/abc319_g

#include <iostream>
#include <vector>
#include <utility>

#include "../../src/algorithm/graph/complement_shortest_path_count.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr long long MOD = 998244353;
    int n, m;
    std::cin >> n >> m;
    std::vector<std::pair<int, int>> excluded_edges;
    excluded_edges.reserve(static_cast<std::size_t>(m));
    for(int i = 0; i < m; ++i){
        int u, v;
        std::cin >> u >> v;
        --u;
        --v;
        excluded_edges.emplace_back(u, v);
    }
    std::cout << complement_shortest_path_count(
        n, excluded_edges, 0, n - 1, MOD
    ) << '\n';
}
