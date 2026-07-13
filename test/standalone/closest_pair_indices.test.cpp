// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/closest_pair_indices.hpp"
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
