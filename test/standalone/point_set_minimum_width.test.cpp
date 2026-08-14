// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/scalar/point_set/point_set_minimum_width.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int count;
    if(!(std::cin >> count)){
        assert(std::abs(point_set_minimum_width({
            {0, 0}, {4, 0}, {4, 1}, {2, 1}, {2, 3}, {0, 3}
        }) - 3.0L) < 1e-12L);
        return 0;
    }
    std::vector<Point> points(static_cast<std::size_t>(count));
    for(Point& point: points) std::cin >> point.x >> point.y;
    std::cout << std::llround(point_set_minimum_width(std::move(points)) * 1e6L)
              << '\n';
}
