// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/scalar/point_set_point_set/polygon_maximum_distance.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int first_count, second_count;
    if(!(std::cin >> first_count >> second_count)){
        const std::vector<Point> first{
            {0, 0}, {3, 0}, {3, 1}, {1, 1}, {1, 4}, {0, 4}
        };
        assert(std::abs(polygon_maximum_distance(first, {{3, 4}}) - 5.0L)
            < 1e-12L);
        return 0;
    }
    std::vector<Point> first(static_cast<std::size_t>(first_count));
    std::vector<Point> second(static_cast<std::size_t>(second_count));
    for(Point& point: first) std::cin >> point.x >> point.y;
    for(Point& point: second) std::cin >> point.x >> point.y;
    std::cout << std::llround(
        polygon_maximum_distance(std::move(first), std::move(second)) * 1e6L
    ) << '\n';
}
