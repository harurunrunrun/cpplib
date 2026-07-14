// competitive-verifier: PROBLEM https://www.spoj.com/problems/GSS4/

#include <algorithm>
#include <iostream>
#include <vector>

#include "../../src/structure/segtree/range_integer_sqrt_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int size;
    int case_number = 0;
    while(std::cin >> size){
        std::vector<long long> values(static_cast<std::size_t>(size));
        for(long long& value: values) std::cin >> value;
        RangeIntegerSqrtSum<100000> tree(values);

        std::cout << "Case #" << ++case_number << ":\n";
        int operation_count;
        std::cin >> operation_count;
        while(operation_count-- != 0){
            int operation, left, right;
            std::cin >> operation >> left >> right;
            if(right < left) std::swap(left, right);
            --left;
            if(operation == 1){
                std::cout << tree.sum(left, right) << '\n';
            }else{
                tree.apply_sqrt(left, right);
            }
        }
        std::cout << '\n';
    }
}
