// competitive-verifier: PROBLEM https://www.spoj.com/problems/PSEGTREE/

#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/segtree/persistent_segtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AddMonoid<long long> psegtree_sum_monoid{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(long long& value : values){
        std::cin >> value;
    }

    auto segtree = std::make_unique<
        PersistentSegtree<psegtree_sum_monoid, 100000, 100000>
    >(values);

    int q;
    std::cin >> q;
    while(q--){
        int type, version, left, right;
        std::cin >> type >> version >> left >> right;
        if(type == 1){
            const int position = left - 1;
            const long long value = segtree->get(version, position) + right;
            segtree->set(version, position, value);
        }else{
            std::cout << segtree->prod(version, left - 1, right) << '\n';
        }
    }
}
