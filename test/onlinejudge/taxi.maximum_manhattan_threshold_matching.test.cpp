// competitive-verifier: PROBLEM https://www.spoj.com/problems/TAXI/

#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include "../../src/algorithm/matching/bipartite/maximum_manhattan_threshold_matching.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count--){
        int left_count, right_count;
        long long speed, pickup_time;
        std::cin >> left_count >> right_count >> speed >> pickup_time;
        std::vector<std::pair<long long, long long>> left_points(
            static_cast<std::size_t>(left_count)
        );
        std::vector<std::pair<long long, long long>> right_points(
            static_cast<std::size_t>(right_count)
        );
        for(auto& [x, y]: left_points) std::cin >> x >> y;
        for(auto& [x, y]: right_points) std::cin >> x >> y;
        const __int128_t distance =
            static_cast<__int128_t>(speed) * pickup_time / 200;
        const long long maximum_distance =
            distance > std::numeric_limits<long long>::max()
                ? std::numeric_limits<long long>::max()
                : static_cast<long long>(distance);
        std::cout << maximum_manhattan_threshold_matching(
            left_points, right_points, maximum_distance
        ) << '\n';
    }
}
