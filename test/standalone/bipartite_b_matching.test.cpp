// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/matching/bipartite/bipartite_b_matching.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int left_size;
        int right_size;
        int edge_count;
        std::cin >> left_size >> right_size >> edge_count;
        std::vector<long long> left_capacity(
            static_cast<std::size_t>(left_size)
        );
        std::vector<long long> right_capacity(
            static_cast<std::size_t>(right_size)
        );
        for(long long& value: left_capacity) std::cin >> value;
        for(long long& value: right_capacity) std::cin >> value;
        BipartiteBMatching<long long> matching(
            left_capacity, right_capacity
        );
        std::vector<long long> edge_capacity(
            static_cast<std::size_t>(edge_count)
        );
        for(int edge = 0; edge < edge_count; ++edge){
            int left;
            int right;
            long long capacity;
            std::cin >> left >> right >> capacity;
            matching.add_edge(left, right, capacity);
            edge_capacity[static_cast<std::size_t>(edge)] = capacity;
        }
        auto result = matching.solve();
        long long total = 0;
        for(int edge = 0; edge < edge_count; ++edge){
            long long value =
                result.edge_flow[static_cast<std::size_t>(edge)];
            if(value < 0
                || edge_capacity[static_cast<std::size_t>(edge)] < value){
                return 2;
            }
            total += value;
        }
        if(total != result.size) return 3;
        for(int left = 0; left < left_size; ++left){
            if(result.left_degree[static_cast<std::size_t>(left)]
                > left_capacity[static_cast<std::size_t>(left)]) return 4;
        }
        for(int right = 0; right < right_size; ++right){
            if(result.right_degree[static_cast<std::size_t>(right)]
                > right_capacity[static_cast<std::size_t>(right)]) return 5;
        }
        std::cout << result.size << '\n';
    }
}
