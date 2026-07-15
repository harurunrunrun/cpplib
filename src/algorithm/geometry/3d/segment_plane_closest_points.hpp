#pragma once

#include <utility>

#include "abs.hpp"
#include "base.hpp"
#include "projection.hpp"
#include "segment_plane_intersection.hpp"
#include "signed_distance.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Segment3& segment,
    const Plane3& plane
){
    const auto intersection = segment_plane_intersection(segment, plane);
    if(intersection) return {*intersection, *intersection};
    const Point3 endpoint =
        std::abs(signed_distance(plane, segment.a)) <=
        std::abs(signed_distance(plane, segment.b))
        ? segment.a : segment.b;
    return {endpoint, projection(plane, endpoint)};
}

inline std::pair<Point3, Point3> closest_points(
    const Plane3& plane,
    const Segment3& segment
){
    const auto result = closest_points(segment, plane);
    return {result.second, result.first};
}
