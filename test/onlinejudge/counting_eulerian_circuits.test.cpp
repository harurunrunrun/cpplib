// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/counting_eulerian_circuits

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/traversal/counting_eulerian_circuits.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [from, to]: edges) std::cin >> from >> to;
    std::cout
        << count_eulerian_circuits<FastModint998244353, 500>(n, edges)
        << '\n';
}
