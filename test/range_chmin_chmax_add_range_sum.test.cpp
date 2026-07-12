// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_chmin_chmax_add_range_sum

#include <iostream>
#include <memory>
#include <vector>
#include "../src/structure/segtree/segtree_beats.hpp"

using ll = long long;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int N, Q;
    std::cin >> N >> Q;

    std::vector<ll> initial(N);
    for(ll& x: initial){
        std::cin >> x;
    }

    auto seg = std::make_unique<SegmentTreeBeats<ll, 200000>>(initial);

    for(int q = 0; q < Q; q++){
        int type, l, r;
        std::cin >> type >> l >> r;

        if(type == 0){
            ll x;
            std::cin >> x;
            seg->range_chmin(l, r, x);
        }else if(type == 1){
            ll x;
            std::cin >> x;
            seg->range_chmax(l, r, x);
        }else if(type == 2){
            ll x;
            std::cin >> x;
            seg->range_add(l, r, x);
        }else{
            std::cout << seg->range_sum(l, r) << '\n';
        }
    }
}
