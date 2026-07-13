// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/enumerate_triangles

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/enumerate_triangles.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr long long mod = 998244353;
    int n, edge_count;
    std::cin >> n >> edge_count;
    std::vector<long long> weight(static_cast<std::size_t>(n));
    for(long long& value: weight) std::cin >> value;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(edge_count));
    for(auto& [left, right]: edges) std::cin >> left >> right;

    long long answer = 0;
    enumerate_triangles(n, edges, [&](int first, int second, int third){
        answer += weight[static_cast<std::size_t>(first)] *
            weight[static_cast<std::size_t>(second)] % mod *
            weight[static_cast<std::size_t>(third)] % mod;
        if(answer >= mod) answer -= mod;
    });
    std::cout << answer << '\n';
}
