// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_add_range_min

#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/segtree/segtree_beats.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size, query_count;
    std::cin >> size >> query_count;
    std::vector<long long> values(static_cast<std::size_t>(size));
    for(long long& value: values) std::cin >> value;
    auto tree = std::make_unique<SegmentTreeBeats<long long, 500000>>(values);
    while(query_count--){
        int type, left, right;
        std::cin >> type >> left >> right;
        if(type == 0){
            long long addend;
            std::cin >> addend;
            tree->range_add(left, right, addend);
        }else{
            std::cout << tree->range_min(left, right) << '\n';
        }
    }
}
