// competitive-verifier: PROBLEM https://www.spoj.com/problems/GSS5/

#include <iostream>
#include <vector>

#include "../../src/structure/segtree/static_two_ranges_maximum_subarray_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int size;
        std::cin >> size;
        std::vector<long long> values(static_cast<std::size_t>(size));
        for(long long& value: values) std::cin >> value;
        StaticTwoRangesMaximumSubarraySum<long long, 10000> tree(values);

        int query_count;
        std::cin >> query_count;
        while(query_count-- != 0){
            int x1, y1, x2, y2;
            std::cin >> x1 >> y1 >> x2 >> y2;
            std::cout << tree.maximum_sum(x1 - 1, y1, x2 - 1, y2) << '\n';
        }
    }
}
