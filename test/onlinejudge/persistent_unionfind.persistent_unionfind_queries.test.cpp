// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/persistent_unionfind

#include <iostream>
#include "../../src/structure/dsu/persistent_unionfind_queries.hpp"

constexpr int MAX_N = 200000;
constexpr int MAX_Q = 200000;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N, Q;
    std::cin >> N >> Q;

    PersistentUnionFindQueries<MAX_N, MAX_Q> dsu(N);
    for(int q = 0; q < Q; q++){
        int type, k, u, v;
        std::cin >> type >> k >> u >> v;

        if(type == 0){
            dsu.merge(k, u, v);
        }else{
            std::cout << dsu.same(k, u, v) << '\n';
        }
    }
}
