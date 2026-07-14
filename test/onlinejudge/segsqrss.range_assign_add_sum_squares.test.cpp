// competitive-verifier: PROBLEM https://www.spoj.com/problems/SEGSQRSS/

#include <iostream>
#include <vector>

#include "../../src/structure/segtree/range_assign_add_sum_squares.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    for(int test_index = 1; test_index <= test_count; ++test_index){
        int size, operation_count;
        std::cin >> size >> operation_count;
        std::vector<long long> values(static_cast<std::size_t>(size));
        for(long long& value: values) std::cin >> value;
        RangeAssignAddSumSquares<long long, 100000> sequence(values);

        std::cout << "Case " << test_index << ":\n";
        while(operation_count-- != 0){
            int operation, left, right;
            std::cin >> operation >> left >> right;
            --left;
            if(operation == 0){
                long long value;
                std::cin >> value;
                sequence.assign(left, right, value);
            }else if(operation == 1){
                long long value;
                std::cin >> value;
                sequence.add(left, right, value);
            }else{
                std::cout << sequence.square_sum(left, right) << '\n';
            }
        }
    }
}
