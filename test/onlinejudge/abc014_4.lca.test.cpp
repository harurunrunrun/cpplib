// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc014/tasks/abc014_4

#include <iostream>

#include "../../src/algorithm/tree/query/lca.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    LowestCommonAncestor lca(n);
    for(int i = 1; i < n; ++i){
        int u, v;
        std::cin >> u >> v;
        lca.add_edge(u - 1, v - 1);
    }
    lca.build();
    int q;
    std::cin >> q;
    while(q--){
        int u, v;
        std::cin >> u >> v;
        std::cout << lca.path_vertex_count(u - 1, v - 1) << '\n';
    }
}
