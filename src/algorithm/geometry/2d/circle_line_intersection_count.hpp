#pragma once

#include "circle_line_cross_points.hpp"

inline int circle_line_intersection_count(
    const Circle& circle,
    const Line& line
){
    return static_cast<int>(circle_line_cross_points(circle, line).size());
}
