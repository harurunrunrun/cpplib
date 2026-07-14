// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/DSL_1_A

#include <iostream>

#include "../../src/structure/dsu/dsu.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int vertex_count, query_count;
    std::cin >> vertex_count >> query_count;
    static DSU<1000000> dsu(vertex_count);
    while(query_count--){
        int type, u, v;
        std::cin >> type >> u >> v;
        if(type == 0) dsu.merge(u, v);
        else std::cout << dsu.same(u, v) << '\n';
    }
}
