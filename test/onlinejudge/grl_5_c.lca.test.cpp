#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/GRL_5_C"

#include <iostream>

#include "../../src/algorithm/tree/lca.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int vertex_count;
    std::cin >> vertex_count;
    LowestCommonAncestor lca(vertex_count);
    for(int vertex = 0; vertex < vertex_count; vertex++){
        int child_count;
        std::cin >> child_count;
        while(child_count--){
            int child;
            std::cin >> child;
            lca.add_edge(vertex, child);
        }
    }
    lca.build(0);
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        int u, v;
        std::cin >> u >> v;
        std::cout << lca.lca(u, v) << '\n';
    }
}
