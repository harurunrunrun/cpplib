// competitive-verifier: PROBLEM https://www.spoj.com/problems/MULTQ3/

#include <iostream>
#include <memory>

#include "../../src/structure/segtree/residue_count_range_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    int query_count;
    std::cin >> size >> query_count;
    using Tree = ResidueCountRangeTree<int, 3, 100000>;
    auto tree = std::make_unique<Tree>(size);

    while(query_count-- != 0){
        int type;
        int left;
        int right;
        std::cin >> type >> left >> right;
        if(type == 0){
            tree->increment(left, right + 1);
        }else{
            std::cout << tree->count_residue(left, right + 1, 0) << '\n';
        }
    }
}
