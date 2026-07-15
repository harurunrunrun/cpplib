#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "is_finite.hpp"
#include "line3_direction.hpp"
#include "line_plane_intersection.hpp"
#include "linear_intersection3.hpp"
#include "on_plane.hpp"
#include "plane3_unit_normal.hpp"

inline LinearIntersection3 line_plane_common_intersection(
    const Line3& line,
    const Plane3& plane
){
    if(!geometry3d_is_finite(line) || !geometry3d_is_finite(plane))[[unlikely]]{
        throw std::invalid_argument(
            "line_plane_common_intersection requires finite inputs"
        );
    }
    Point3 direction = line3_direction(line);
    const long double scale = std::max({
        std::abs(direction.x), std::abs(direction.y), std::abs(direction.z)
    });
    direction /= scale;
    const Point3 normal = plane3_unit_normal(plane);
    if(geometry3d_sign(dot(direction, normal)) == 0){
        if(on_plane(plane, line.a)) return line;
        return std::monostate{};
    }
    return line_plane_intersection(line, plane);
}
