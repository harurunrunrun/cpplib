// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/persistent_unionfind

#include <iostream>
#include <vector>
#include "../../src/structure/dsu/persistent_dsu.hpp"

constexpr int MAX_N = 200000;
constexpr int MAX_Q = 200000;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N, Q;
    std::cin >> N >> Q;

    PersistentDSU<MAX_N, MAX_Q> dsu(N);
    std::vector<int> version_at(Q + 1);
    version_at[0] = 0;

    for(int q = 0; q < Q; q++){
        int type, k, u, v;
        std::cin >> type >> k >> u >> v;

        int base = version_at[k + 1];
        if(type == 0){
            version_at[q + 1] = dsu.merge(base, u, v);
        }else{
            std::cout << dsu.same(base, u, v) << '\n';
            version_at[q + 1] = base;
        }
    }
}
