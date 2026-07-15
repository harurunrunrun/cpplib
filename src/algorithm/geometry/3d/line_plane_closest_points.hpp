#pragma once

#include <utility>

#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "line3_direction.hpp"
#include "line_plane_intersection.hpp"
#include "plane3_unit_normal.hpp"
#include "projection.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Line3& line,
    const Plane3& plane
){
    const Point3 direction = line3_direction(line);
    const Point3 normal = plane3_unit_normal(plane);
    if(geometry3d_sign(dot(direction, normal)) != 0){
        const Point3 point = line_plane_intersection(line, plane);
        return {point, point};
    }
    return {line.a, projection(plane, line.a)};
}

inline std::pair<Point3, Point3> closest_points(
    const Plane3& plane,
    const Line3& line
){
    const auto result = closest_points(line, plane);
    return {result.second, result.first};
}
