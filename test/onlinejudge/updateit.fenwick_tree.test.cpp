// competitive-verifier: PROBLEM https://www.spoj.com/problems/UPDATEIT/

#include <iostream>
#include <memory>

#include "../../src/structure/other/fenwick_tree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int size, update_count;
        std::cin >> size >> update_count;
        auto difference = std::make_unique<FenwickTree<long long, 10000>>(size);
        while(update_count-- != 0){
            int left, right;
            long long value;
            std::cin >> left >> right >> value;
            difference->add(left, value);
            if(right + 1 < size){
                difference->add(right + 1, -value);
            }
        }

        int query_count;
        std::cin >> query_count;
        while(query_count-- != 0){
            int index;
            std::cin >> index;
            std::cout << difference->prefix_sum(index + 1) << '\n';
        }
    }
}
