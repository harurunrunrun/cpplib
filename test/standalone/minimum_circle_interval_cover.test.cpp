// competitive-verifier: STANDALONE

#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/minimum_circle_interval_cover.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    const long double turn = 2.0L * std::acos(-1.0L);
    while(test_count-- > 0){
        int interval_count, tick_count;
        std::cin >> interval_count >> tick_count;
        std::vector<std::pair<long double, long double>> intervals;
        intervals.reserve(static_cast<std::size_t>(interval_count));
        for(int index = 0; index < interval_count; ++index){
            long long start, end;
            std::cin >> start >> end;
            intervals.emplace_back(
                static_cast<long double>(start) * turn / tick_count,
                static_cast<long double>(end) * turn / tick_count
            );
        }
        std::cout << minimum_circle_interval_cover(intervals) << '\n';
    }
}
