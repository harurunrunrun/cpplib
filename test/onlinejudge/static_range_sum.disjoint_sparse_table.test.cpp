// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_range_sum

#include <iostream>
#include <vector>

#include "../../src/structure/other/disjoint_sparse_table.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AddMonoid<long long> add_monoid{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for(int i = 0; i < n; i++){
        std::cin >> a[i];
    }

    DisjointSparseTable<add_monoid, 500000> dst(a);
    while(q--){
        int l, r;
        std::cin >> l >> r;
        std::cout << dst.prod(l, r) << '\n';
    }
}
