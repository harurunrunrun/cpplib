// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/dynamic_sequence_range_affine_range_sum

#include <iostream>
#include <vector>
#include "../../src/structure/bbst/sequence/lazy_reversible_splay_tree.hpp"
#include "../../src/structure/modint/modint.hpp"
#include "../../src/structure/types/monoid/common_monoids.hpp"

using mint = Modint998244353;
constexpr AffineSumMonoidAct<mint> affine_sum;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    std::vector<mint> values(n);
    for(auto& x: values) std::cin >> x;
    LazyReversibleSplayTree<affine_sum, 1'000'000> sequence(values);
    while(q--){
        int type, l, r;
        std::cin >> type >> l;
        if(type == 0){ mint x; std::cin >> x; sequence.insert(l, x); }
        else if(type == 1) sequence.erase(l);
        else{
            std::cin >> r;
            if(type == 2) sequence.reverse(l, r);
            else if(type == 3){
                mint b, c; std::cin >> b >> c;
                sequence.apply(l, r, MonoidAffine<mint>{b, c});
            }else std::cout << sequence.prod(l, r) << '\n';
        }
    }
}
