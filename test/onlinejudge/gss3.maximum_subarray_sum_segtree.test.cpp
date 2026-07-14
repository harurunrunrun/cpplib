// competitive-verifier: PROBLEM https://www.spoj.com/problems/GSS3/

#include <iostream>
#include <vector>

#include "../../src/structure/segtree/maximum_subarray_sum_segtree.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<long long> values(static_cast<std::size_t>(size));
    for(long long& value: values) std::cin >> value;
    MaximumSubarraySumSegtree<long long, 50000> sequence(values);

    int operation_count;
    std::cin >> operation_count;
    while(operation_count-- != 0){
        int operation, left;
        long long right;
        std::cin >> operation >> left >> right;
        if(operation == 0) sequence.set(left - 1, right);
        else std::cout << sequence.maximum_sum(
            left - 1, static_cast<int>(right)
        ) << '\n';
    }
}
