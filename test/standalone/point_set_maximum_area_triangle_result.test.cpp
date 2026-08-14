// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <iostream>

#include "../../src/algorithm/geometry/2d/type/definition/point_set_maximum_area_triangle_result.hpp"

int main(){
    long double doubled_area = 12.0L;
    std::cin >> doubled_area;
    const PointSetMaximumAreaTriangleResult result{
        {{{0, 0}, {4, 0}, {0, 3}}}, doubled_area, true
    };
    assert(result.valid);
    std::cout << std::llround(result.area() * 1e6L) << '\n';
}
