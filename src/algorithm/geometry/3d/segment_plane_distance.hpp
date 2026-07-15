#pragma once

#include "abs.hpp"
#include "base.hpp"
#include "segment_plane_closest_points.hpp"

inline long double distance(const Segment3& segment, const Plane3& plane){
    const auto points = closest_points(segment, plane);
    return abs(points.first - points.second);
}

inline long double distance(const Plane3& plane, const Segment3& segment){
    return distance(segment, plane);
}
