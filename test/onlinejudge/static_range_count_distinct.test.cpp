// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_range_count_distinct

#include <iostream>
#include <vector>

#include "../../src/algorithm/range/static_range_count_distinct.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, query_count;
    std::cin >> n >> query_count;
    std::vector<int> values(static_cast<std::size_t>(n));
    for(int& value: values) std::cin >> value;
    StaticRangeCountDistinct<int, 500000> structure(values);
    while(query_count-- > 0){
        int left, right;
        std::cin >> left >> right;
        std::cout << structure.count(left, right) << '\n';
    }
}
