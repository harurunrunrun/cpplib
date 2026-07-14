// competitive-verifier: PROBLEM https://www.spoj.com/problems/GOT/

#include <iostream>
#include <vector>

#include "../../src/algorithm/tree/tree_path_order_statistics.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count, query_count;
    while(std::cin >> vertex_count >> query_count){
        std::vector<int> values(static_cast<std::size_t>(vertex_count));
        for(int& value: values){
            std::cin >> value;
        }

        TreePathOrderStatistics<int, 100000> statistics(vertex_count);
        for(int edge = 1; edge < vertex_count; ++edge){
            int left, right;
            std::cin >> left >> right;
            statistics.add_edge(left - 1, right - 1);
        }
        statistics.build(values);

        while(query_count-- != 0){
            int left, right, target;
            std::cin >> left >> right >> target;
            std::cout << (
                statistics.count_equal(left - 1, right - 1, target) != 0
                    ? "Find"
                    : "NotFind"
            ) << '\n';
        }
        std::cout << '\n';
    }
}
