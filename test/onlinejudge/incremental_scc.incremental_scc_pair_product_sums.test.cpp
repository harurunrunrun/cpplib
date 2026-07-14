// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/incremental_scc

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/incremental_scc_pair_product_sums.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;
    std::vector<FastModint998244353> sum(static_cast<std::size_t>(n));
    for(auto& value: sum) std::cin >> value;
    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));
    for(auto& [from, to]: edges) std::cin >> from >> to;

    for(const auto answer:
        incremental_scc_pair_product_sums(sum, edges)){
        std::cout << answer << '\n';
    }
}
