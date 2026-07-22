#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_COLLECTION_LINE_PLANE_CLOSEST_POINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_COLLECTION_LINE_PLANE_CLOSEST_POINTS_HPP_INCLUDED

#include <utility>

#include "../core/geometry_primitives.hpp"
#include "../scalar/dot.hpp"
#include "../predicate/geometry3d_sign.hpp"
#include "../point/line3_direction.hpp"
#include "../point/line_plane_intersection.hpp"
#include "../point/plane3_unit_normal.hpp"
#include "../point/projection.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_COLLECTION_LINE_PLANE_CLOSEST_POINTS_HPP_INCLUDED
