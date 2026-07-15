#pragma once

#include "abs.hpp"
#include "base.hpp"
#include "line_segment_closest_points.hpp"

inline long double distance(const Line3& line, const Segment3& segment){
    const auto points = closest_points(line, segment);
    return abs(points.first - points.second);
}

inline long double distance(const Segment3& segment, const Line3& line){
    return distance(line, segment);
}
