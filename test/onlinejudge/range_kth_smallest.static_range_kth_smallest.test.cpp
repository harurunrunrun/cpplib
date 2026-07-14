// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_kth_smallest

#include <iostream>
#include <vector>

#include "../../src/structure/other/static_range_kth_smallest.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size, query_count;
    std::cin >> size >> query_count;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values){
        std::cin >> value;
    }

    StaticRangeKthSmallest<int, 200000> statistics(values);
    while(query_count-- != 0){
        int left, right, order;
        std::cin >> left >> right >> order;
        std::cout << statistics.kth_smallest(left, right, order) << '\n';
    }
}
