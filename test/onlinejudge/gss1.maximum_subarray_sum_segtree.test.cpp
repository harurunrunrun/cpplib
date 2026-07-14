// competitive-verifier: PROBLEM https://www.spoj.com/problems/GSS1/

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

    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        int left, right;
        std::cin >> left >> right;
        std::cout << sequence.maximum_sum(left - 1, right) << '\n';
    }
}
