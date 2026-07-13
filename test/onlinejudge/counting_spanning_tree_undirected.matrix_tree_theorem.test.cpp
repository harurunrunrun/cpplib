// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/counting_spanning_tree_undirected

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/matrix_tree_theorem.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;
    std::vector<MatrixTreeEdge<FastModint998244353>> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int index = 0; index < m; index++){
        int from, to;
        std::cin >> from >> to;
        edges.push_back({from, to, 1});
    }
    std::cout
        << count_weighted_undirected_spanning_trees<
            FastModint998244353, 500
        >(n, edges)
        << '\n';
}
