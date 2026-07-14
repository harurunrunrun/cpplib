// competitive-verifier: PROBLEM https://www.spoj.com/problems/PSEGTREE/

#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/segtree/persistent_point_add_range_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(long long& value : values){
        std::cin >> value;
    }

    using Tree = PersistentPointAddRangeSum<long long, 100000, 100000>;
    auto tree = std::make_unique<Tree>(values);

    int q;
    std::cin >> q;
    while(q--){
        int type, version, left, right;
        std::cin >> type >> version >> left >> right;
        if(type == 1){
            tree->add(version, left - 1, right);
        }else{
            std::cout << tree->sum(version, left - 1, right) << '\n';
        }
    }
}
