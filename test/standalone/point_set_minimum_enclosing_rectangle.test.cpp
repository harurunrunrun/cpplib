// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/result/point_set/point_set_minimum_enclosing_rectangle.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int count;
    if(!(std::cin >> count)){
        const auto result = point_set_minimum_enclosing_rectangle({
            {0, 0}, {4, 0}, {4, 1}, {2, 1}, {2, 3}, {0, 3}
        });
        assert(result.valid);
        assert(std::abs(result.area - 12.0L) < 1e-12L);
        return 0;
    }
    std::vector<Point> points(static_cast<std::size_t>(count));
    for(Point& point: points) std::cin >> point.x >> point.y;
    const auto result = point_set_minimum_enclosing_rectangle(std::move(points));
    std::cout << result.valid << ' '
              << std::llround(result.area * 1e6L) << '\n';
}
