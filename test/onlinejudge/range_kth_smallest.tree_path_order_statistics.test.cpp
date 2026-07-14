// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_kth_smallest

#include <iostream>
#include <vector>

#include "../../src/algorithm/tree/tree_path_order_statistics.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size, query_count;
    std::cin >> size >> query_count;
    std::vector<int> values(static_cast<std::size_t>(size));
    for(auto& value: values){
        std::cin >> value;
    }

    TreePathOrderStatistics<int, 200000> statistics(size);
    for(int vertex = 1; vertex < size; ++vertex){
        statistics.add_edge(vertex - 1, vertex);
    }
    statistics.build(values);

    while(query_count-- != 0){
        int left, right, order;
        std::cin >> left >> right >> order;
        std::cout << statistics.kth_smallest(left, right - 1, order) << '\n';
    }
}
