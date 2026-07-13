#pragma once

#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/convex_polygon_diameter.hpp"
#include "../../src/algorithm/geometry/convex_polygon_farthest_vertices.hpp"
#include "../../src/algorithm/geometry/convex_polygon_maximum_area_triangle.hpp"
#include "../../src/algorithm/geometry/convex_polygon_support_vertex.hpp"
#include "../../src/algorithm/geometry/convex_polygon_tangents.hpp"
#include "../../src/algorithm/geometry/rotating_calipers.hpp"

inline std::vector<Point> read_calipers_polygon(){
    std::size_t count;
    std::cin >> count;
    std::vector<Point> polygon(count);
    for(Point& point: polygon) std::cin >> point.x >> point.y;
    return polygon;
}

inline long long rounded_calipers_value(long double value){
    if(value < static_cast<long double>(std::numeric_limits<long long>::min())
        || value > static_cast<long double>(std::numeric_limits<long long>::max())){
        throw std::overflow_error("the test value does not fit in long long");
    }
    return std::llround(value);
}

inline void check_calipers_equal(long double first, long double second){
    if(rotating_calipers_detail::scalar_compare(first, second) != 0){
        throw std::runtime_error("inconsistent rotating-calipers result");
    }
}
