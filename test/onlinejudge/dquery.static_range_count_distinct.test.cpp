// competitive-verifier: PROBLEM https://www.spoj.com/problems/DQUERY/

#include <iostream>
#include <vector>

#include "../../src/algorithm/range/static_range_count_distinct.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;
    StaticRangeCountDistinct<int, 30000> distinct(values);

    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        int left, right;
        std::cin >> left >> right;
        std::cout << distinct.count(left - 1, right) << '\n';
    }
}
