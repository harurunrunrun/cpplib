// competitive-verifier: PROBLEM https://www.spoj.com/problems/ZQUERY/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/range/maximum_zero_sum_subarray_lengths.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size, query_count;
    std::cin >> size >> query_count;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;
    std::vector<std::pair<int, int>> ranges;
    ranges.reserve(static_cast<std::size_t>(query_count));
    while(query_count-- != 0){
        int left, right;
        std::cin >> left >> right;
        ranges.emplace_back(left - 1, right);
    }
    for(int answer: maximum_zero_sum_subarray_lengths(values, ranges)){
        std::cout << answer << '\n';
    }
}
