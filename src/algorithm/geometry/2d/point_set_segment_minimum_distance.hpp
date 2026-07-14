#pragma once

#include <algorithm>
#include <limits>
#include <vector>

#include "distance.hpp"

inline long double point_set_segment_minimum_distance(
    const std::vector<Point>& points,
    const Segment& segment
){
    long double result = std::numeric_limits<long double>::infinity();
    for(const Point& point: points){
        result = std::min(result, distance_segment_point(segment, point));
    }
    return result;
}
