// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_range_sum_with_upper_bound

#include <iostream>
#include <vector>

#include "../../src/structure/wavelet_matrix/static_range_count_sum_leq.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size, query_count;
    std::cin >> size >> query_count;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int index = 0; index < size; ++index){
        std::cin >> values[static_cast<std::size_t>(index)];
    }
    StaticRangeCountSumLeq<int, long long, 500000> matrix(values);
    while(query_count--){
        int left, right, upper;
        std::cin >> left >> right >> upper;
        const auto result = matrix.query(left, right, upper);
        std::cout << result.count << ' ' << result.sum << '\n';
    }
}
