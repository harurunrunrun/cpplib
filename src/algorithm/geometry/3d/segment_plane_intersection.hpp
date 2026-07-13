#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "abs.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "on_plane.hpp"
#include "plane3_unit_normal.hpp"

inline std::optional<Point3> segment_plane_intersection(
    const Segment3& segment,
    const Plane3& plane
){
    const Point3 direction = segment.b - segment.a;
    if(geometry3d_sign(abs(direction)) == 0){
        return on_plane(plane, segment.a)
            ? std::optional<Point3>(segment.a) : std::nullopt;
    }
    const Point3 normal = plane3_unit_normal(plane);
    const long double denominator = dot(direction, normal);
    if(geometry3d_sign(denominator) == 0) return std::nullopt;
    const long double parameter =
        dot(plane.point - segment.a, normal) / denominator;
    if(geometry3d_sign(parameter) < 0 || geometry3d_sign(parameter - 1) > 0){
        return std::nullopt;
    }
    return segment.a + direction * parameter;
}
