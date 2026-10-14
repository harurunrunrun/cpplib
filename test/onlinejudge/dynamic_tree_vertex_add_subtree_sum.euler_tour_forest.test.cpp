// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/dynamic_tree_vertex_add_subtree_sum

#include <iostream>
#include <vector>
#include "../../src/structure/tree/dynamic_forest/euler_tour_forest.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<long long> a(n); for(auto& x: a) std::cin >> x;
    EulerTourForest<long long> tree(a);
    for(int i = 1, u, v; i < n; ++i){ std::cin >> u >> v; tree.link(u, v); }
    while(q--){
        int type; std::cin >> type;
        if(type == 0){ int u,v,w,x; std::cin >> u >> v >> w >> x; tree.cut(u,v); tree.link(w,x); }
        else if(type == 1){ int v; long long x; std::cin >> v >> x; tree.vertex_add(v,x); }
        else{ int v,p; std::cin >> v >> p; std::cout << tree.subtree_sum(v,p) << '\n'; }
    }
}
