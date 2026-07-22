// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/query/furthest_pair.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    assert(!furthest_pair(std::vector<std::pair<long long, long long>>{}).exists());
    assert(!furthest_pair(
        std::vector<std::pair<long long, long long>>{{3, 4}}
    ).exists());

    const long long minimum = std::numeric_limits<long long>::min();
    const long long maximum = std::numeric_limits<long long>::max();
    const ExactInteger full_difference =
        std::numeric_limits<unsigned long long>::max();
    const ExactInteger full_squared_distance =
        2 * full_difference * full_difference;
    const FurthestPairResult signed_boundary = furthest_pair(
        std::vector<std::pair<long long, long long>>{
            {minimum, minimum}, {maximum, maximum}
        }
    );
    assert(signed_boundary.first == 0 && signed_boundary.second == 1);
    assert(signed_boundary.squared_distance == full_squared_distance);
    const FurthestPairResult unsigned_boundary = furthest_pair(
        std::vector<std::pair<unsigned long long, unsigned long long>>{
            {0, 0},
            {
                std::numeric_limits<unsigned long long>::max(),
                std::numeric_limits<unsigned long long>::max()
            }
        }
    );
    assert(unsigned_boundary.squared_distance == full_squared_distance);

    const FurthestPairResult overflow_order = furthest_pair(
        std::vector<std::pair<long long, long long>>{
            {minimum, 0}, {maximum, 1LL << 33}, {minimum, 1LL << 34}
        }
    );
    assert(overflow_order.first == 0 && overflow_order.second == 1);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        std::size_t size;
        std::cin >> size;
        std::vector<std::pair<long long, long long>> points(size);
        for(auto& [x, y]: points) std::cin >> x >> y;
        const FurthestPairResult result = furthest_pair(points);
        if(result.exists()){
            std::cout << result.first << ' ' << result.second << '\n';
        }else{
            std::cout << "-1 -1\n";
        }
    }
}
