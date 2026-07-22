// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/lca

#include <iostream>
#include "../../src/algorithm/tree/query/lca.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    LowestCommonAncestor lca(n);
    for(int vertex = 1; vertex < n; ++vertex){
        int parent;
        std::cin >> parent;
        lca.add_edge(parent, vertex);
    }
    lca.build(0);
    while(q--){
        int u, v;
        std::cin >> u >> v;
        std::cout << lca.lca(u, v) << '\n';
    }
}
