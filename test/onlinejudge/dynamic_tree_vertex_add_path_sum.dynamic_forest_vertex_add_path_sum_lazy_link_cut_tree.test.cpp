// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/dynamic_tree_vertex_add_path_sum

#include <iostream>
#include <vector>
#include "../../src/structure/tree/dynamic_forest_vertex_add_path_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for(auto& x: a) std::cin >> x;
    DynamicForestVertexAddPathSumLazyLinkCutTree<long long, 200000> tree(a);
    for(int i = 0; i < n - 1; i++){
        int u, v;
        std::cin >> u >> v;
        tree.link(u, v);
    }
    while(q--){
        int type;
        std::cin >> type;
        if(type == 0){
            int u, v, w, x;
            std::cin >> u >> v >> w >> x;
            tree.replace_edge(u, v, w, x);
        }else if(type == 1){
            int p;
            long long x;
            std::cin >> p >> x;
            tree.add_vertex(p, x);
        }else{
            int u, v;
            std::cin >> u >> v;
            std::cout << tree.path_sum(u, v) << '\n';
        }
    }
}
