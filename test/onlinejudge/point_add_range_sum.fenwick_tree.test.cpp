// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/point_add_range_sum

#include <iostream>
#include <vector>

#include "../../src/structure/other/fenwick_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for(int i = 0; i < n; i++){
        std::cin >> a[i];
    }

    FenwickTree<long long, 500000> fw(a);
    while(q--){
        int t;
        std::cin >> t;
        if(t == 0){
            int p;
            long long x;
            std::cin >> p >> x;
            fw.add(p, x);
        }else{
            int l, r;
            std::cin >> l >> r;
            std::cout << fw.sum(l, r) << '\n';
        }
    }
}
