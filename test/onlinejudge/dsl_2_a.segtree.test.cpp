#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/DSL_2_A"

#include <iostream>
#include <limits>

#include "../../src/structure/segtree/segtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr MinMonoid<int> aoj_range_min_monoid{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int size, query_count;
    std::cin >> size >> query_count;
    static Segtree<aoj_range_min_monoid, 1000000> segtree(size);
    while(query_count--){
        int type, x, y;
        std::cin >> type >> x >> y;
        if(type == 0) segtree.set(x, y);
        else std::cout << segtree.prod(x, y + 1) << '\n';
    }
}
