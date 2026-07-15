#pragma once

#include "abs.hpp"
#include "base.hpp"
#include "line_plane_closest_points.hpp"

inline long double distance(const Line3& line, const Plane3& plane){
    const auto points = closest_points(line, plane);
    return abs(points.first - points.second);
}

inline long double distance(const Plane3& plane, const Line3& line){
    return distance(line, plane);
}
