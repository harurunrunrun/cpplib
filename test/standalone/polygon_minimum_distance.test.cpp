// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/scalar/point_set_point_set/polygon_minimum_distance.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int first_count, second_count;
    if(!(std::cin >> first_count >> second_count)){
        const std::vector<Point> concave{
            {0, 0}, {3, 0}, {3, 1}, {1, 1}, {1, 3}, {0, 3}
        };
        const std::vector<Point> separated{
            {5, 0}, {6, 0}, {6, 1}, {5, 1}
        };
        assert(std::abs(polygon_minimum_distance(concave, separated) - 2.0L)
            < 1e-12L);
        assert(polygon_minimum_distance(
            concave, {{0.2L, 0.2L}, {0.8L, 0.2L}, {0.8L, 0.8L}, {0.2L, 0.8L}}
        ) == 0.0L);
        return 0;
    }
    std::vector<Point> first(static_cast<std::size_t>(first_count));
    std::vector<Point> second(static_cast<std::size_t>(second_count));
    for(Point& point: first) std::cin >> point.x >> point.y;
    for(Point& point: second) std::cin >> point.x >> point.y;
    std::cout << std::llround(
        polygon_minimum_distance(first, second) * 1e6L
    ) << '\n';
}
