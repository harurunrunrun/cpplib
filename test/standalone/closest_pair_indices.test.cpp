// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/closest_pair_indices.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    assert(!closest_pair_indices(
        std::vector<std::pair<long long, long long>>{}
    ).exists());
    assert(!closest_pair_indices(
        std::vector<std::pair<long long, long long>>{{0, 0}}
    ).exists());

    const long long minimum = std::numeric_limits<long long>::min();
    const long long maximum = std::numeric_limits<long long>::max();
    const ExactInteger full_difference =
        std::numeric_limits<unsigned long long>::max();
    const ExactInteger full_squared_distance =
        2 * full_difference * full_difference;
    const ClosestPairResult signed_boundary = closest_pair_indices(
        std::vector<std::pair<long long, long long>>{
            {minimum, minimum}, {maximum, maximum}
        }
    );
    assert(signed_boundary.first == 0 && signed_boundary.second == 1);
    assert(signed_boundary.squared_distance == full_squared_distance);
    const ClosestPairResult unsigned_boundary = closest_pair_indices(
        std::vector<std::pair<unsigned long long, unsigned long long>>{
            {0, 0},
            {
                std::numeric_limits<unsigned long long>::max(),
                std::numeric_limits<unsigned long long>::max()
            }
        }
    );
    assert(unsigned_boundary.squared_distance == full_squared_distance);

    const ClosestPairResult overflow_order = closest_pair_indices(
        std::vector<std::pair<long long, long long>>{
            {minimum, 0},
            {maximum, 1LL << 33},
            {minimum, 1LL << 34},
        }
    );
    assert(overflow_order.first == 0 && overflow_order.second == 2);
    assert(
        overflow_order.squared_distance
        == (ExactInteger(1) << 68)
    );

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::size_t size;
        std::cin >> size;
        std::vector<std::pair<long long, long long>> points(size);
        for(auto& [x, y]: points) std::cin >> x >> y;
        const ClosestPairResult result = closest_pair_indices(points);
        std::cout << result.first << ' ' << result.second << '\n';
    }
}
