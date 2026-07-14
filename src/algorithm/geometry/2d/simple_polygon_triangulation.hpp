#pragma once

#include <vector>

#include "ear_clipping_triangulation.hpp"

inline std::vector<EarClippingTriangle> simple_polygon_triangulation(
    const std::vector<Point>& polygon
){
    return ear_clipping_triangulation(polygon);
}
