// competitive-verifier: PROBLEM https://www.spoj.com/problems/GSS2/

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/range/maximum_distinct_subarray_sums.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int element_count;
    std::cin >> element_count;
    std::vector<long long> values(static_cast<std::size_t>(element_count));
    for(long long& value: values) std::cin >> value;
    int query_count;
    std::cin >> query_count;
    std::vector<std::pair<int, int>> queries;
    queries.reserve(static_cast<std::size_t>(query_count));
    while(query_count-- != 0){
        int left, right;
        std::cin >> left >> right;
        queries.emplace_back(left - 1, right);
    }
    for(long long answer: maximum_distinct_subarray_sums<long long, 100000>(
        values, queries
    )) std::cout << answer << '\n';
}
