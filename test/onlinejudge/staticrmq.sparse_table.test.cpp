// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/staticrmq

#include <iostream>
#include <vector>

#include "../../src/structure/other/sparse_table.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr MinMonoid<int> min_monoid{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    std::cin >> n >> q;
    std::vector<int> a(n);
    for(int i = 0; i < n; i++){
        std::cin >> a[i];
    }

    SparseTable<min_monoid, 500000> st(a);
    while(q--){
        int l, r;
        std::cin >> l >> r;
        std::cout << st.prod(l, r) << '\n';
    }
}
