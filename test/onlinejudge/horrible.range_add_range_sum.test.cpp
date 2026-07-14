// competitive-verifier: PROBLEM https://www.spoj.com/problems/HORRIBLE/

#include <iostream>

#include "../../src/structure/segtree/range_add_range_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int size, command_count;
        std::cin >> size >> command_count;
        RangeAddRangeSum<long long, 100000> sequence(size);
        while(command_count-- != 0){
            int type, left, right;
            std::cin >> type >> left >> right;
            --left;
            if(type == 0){
                long long value;
                std::cin >> value;
                sequence.add(left, right, value);
            }else{
                std::cout << sequence.sum(left, right) << '\n';
            }
        }
    }
}
