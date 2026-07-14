#pragma once

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

#include "distance.hpp"

inline long double point_set_line_minimum_distance(
    const std::vector<Point>& points,
    const Line& line
){
    if(geometry_sign(abs(line.b - line.a)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    long double result = std::numeric_limits<long double>::infinity();
    for(const Point& point: points){
        result = std::min(result, distance(line, point));
    }
    return result;
}
