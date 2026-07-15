#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "is_finite.hpp"
#include "linear_intersection3.hpp"
#include "on_plane.hpp"
#include "plane3_unit_normal.hpp"
#include "segment_plane_intersection.hpp"

inline LinearIntersection3 segment_plane_common_intersection(
    const Segment3& segment,
    const Plane3& plane
){
    if(!geometry3d_is_finite(segment) || !geometry3d_is_finite(plane))
        [[unlikely]]{
        throw std::invalid_argument(
            "segment_plane_common_intersection requires finite inputs"
        );
    }
    Point3 direction = segment.b - segment.a;
    const long double scale = std::max({
        std::abs(direction.x), std::abs(direction.y), std::abs(direction.z)
    });
    if(scale == 0.0L){
        if(on_plane(plane, segment.a)) return segment.a;
        return std::monostate{};
    }
    direction /= scale;
    const Point3 normal = plane3_unit_normal(plane);
    if(geometry3d_sign(dot(direction, normal)) == 0){
        if(on_plane(plane, segment.a)) return segment;
        return std::monostate{};
    }
    const auto point = segment_plane_intersection(segment, plane);
    if(point) return *point;
    return std::monostate{};
}
