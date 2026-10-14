// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_linear_add_range_min

#include <cstdint>
#include <iostream>
#include <vector>
#include "../../src/structure/range_query/extrema/range_linear_add_min.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<std::int64_t> a(n); for(auto& x: a) std::cin >> x;
    RangeLinearAddMin tree(a);
    while(q--){
        int t, l, r; std::cin >> t >> l >> r;
        if(t == 0){ std::int64_t b, c; std::cin >> b >> c; tree.add(l, r, b, c); }
        else std::cout << tree.minimum(l, r) << '\n';
    }
}
