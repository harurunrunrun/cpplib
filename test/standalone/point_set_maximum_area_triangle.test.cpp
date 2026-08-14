// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/result/point_set/point_set_maximum_area_triangle.hpp"

int main(){
    int count;
    std::vector<Point> points;
    if(std::cin >> count){
        points.resize(static_cast<std::size_t>(count));
        for(Point& point: points) std::cin >> point.x >> point.y;
    }else{
        points = {{0, 0}, {3, 0}, {3, 1}, {1, 1}, {1, 3}, {0, 3}};
    }
    const auto result = point_set_maximum_area_triangle(std::move(points));
    assert(result.valid);
    assert(std::abs(result.area() - 4.5L) < 1e-12L);
    std::cout << std::llround(result.area() * 1e6L) << '\n';
}
