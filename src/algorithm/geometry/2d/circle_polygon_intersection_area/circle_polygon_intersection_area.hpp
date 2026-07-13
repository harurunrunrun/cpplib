#pragma once

#include <cmath>
#include <vector>

#include "signed_circle_polygon_intersection_area.hpp"

inline long double circle_polygon_intersection_area(
    const Circle& circle,
    const std::vector<Point>& polygon
){
    return std::abs(signed_circle_polygon_intersection_area(circle, polygon));
}
