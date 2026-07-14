// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/persistent_unionfind

#include <iostream>
#include <vector>
#include "../../src/structure/dsu/offline_persistent_dsu.hpp"

constexpr int MAX_N = 200000;
constexpr int MAX_Q = 200000;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N, Q;
    std::cin >> N >> Q;

    OfflinePersistentDSU<MAX_N, MAX_Q, MAX_Q> dsu(N);
    std::vector<int> version_at(static_cast<std::size_t>(Q + 1));
    for(int q = 0; q < Q; ++q){
        int type, k, u, v;
        std::cin >> type >> k >> u >> v;
        const int base = version_at[static_cast<std::size_t>(k + 1)];
        if(type == 0){
            version_at[static_cast<std::size_t>(q + 1)] =
                dsu.merge(base, u, v);
        }else{
            dsu.query_same(base, u, v);
            version_at[static_cast<std::size_t>(q + 1)] = base;
        }
    }
    for(int answer: dsu.solve()) std::cout << answer << '\n';
}
