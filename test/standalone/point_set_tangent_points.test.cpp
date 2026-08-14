// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/result/point_set_point/point_set_tangent_points.hpp"

int main(){
    int count;
    std::vector<Point> points;
    Point source{4, 4};
    if(std::cin >> count){
        points.resize(static_cast<std::size_t>(count));
        for(Point& point: points) std::cin >> point.x >> point.y;
        std::cin >> source.x >> source.y;
    }else{
        points = {{0, 0}, {3, 0}, {3, 1}, {1, 1}, {1, 3}, {0, 3}};
    }
    auto result = point_set_tangent_points(std::move(points), source);
    assert(result.has_value());
    std::sort(result->begin(), result->end());
    std::cout << (*result)[0].x << ' ' << (*result)[0].y << ' '
        << (*result)[1].x << ' ' << (*result)[1].y << '\n';
}
