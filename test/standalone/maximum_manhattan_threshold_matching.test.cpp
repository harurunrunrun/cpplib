// competitive-verifier: STANDALONE

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/matching/bipartite/maximum_manhattan_threshold_matching.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int left_size, right_size;
        long long maximum_distance;
        std::cin >> left_size >> right_size >> maximum_distance;
        std::vector<std::pair<long long, long long>> left_points(
            static_cast<std::size_t>(left_size)
        );
        std::vector<std::pair<long long, long long>> right_points(
            static_cast<std::size_t>(right_size)
        );
        for(auto& [x, y]: left_points) std::cin >> x >> y;
        for(auto& [x, y]: right_points) std::cin >> x >> y;
        std::cout << maximum_manhattan_threshold_matching(
            left_points, right_points, maximum_distance
        ) << '\n';
    }
}
