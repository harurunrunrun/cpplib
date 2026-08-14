// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/scalar/point_set/polygon_diameter.hpp"

int main(){
    int count;
    std::vector<Point> polygon;
    if(std::cin >> count){
        polygon.resize(static_cast<std::size_t>(count));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
    }else{
        polygon = {{0, 0}, {3, 0}, {3, 1}, {1, 1}, {1, 4}, {0, 4}};
    }
    const long double result = polygon_diameter(std::move(polygon));
    assert(std::abs(result - 5.0L) < 1e-12L);
    std::cout << std::llround(result * 1e6L) << '\n';
}
