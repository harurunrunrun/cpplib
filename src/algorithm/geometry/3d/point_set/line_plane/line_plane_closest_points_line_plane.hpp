#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_LINE_PLANE_LINE_PLANE_CLOSEST_POINTS_LINE_PLANE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_LINE_PLANE_LINE_PLANE_CLOSEST_POINTS_LINE_PLANE_HPP_INCLUDED

#include <utility>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../scalar/point_point/dot.hpp"
#include "../../integer/scalar/geometry3d_sign.hpp"
#include "../../point/line/line3_direction.hpp"
#include "../../point/line_plane/line_plane_intersection.hpp"
#include "../../point/plane/plane3_unit_normal.hpp"
#include "../../aggregate/all/projection.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_LINE_PLANE_LINE_PLANE_CLOSEST_POINTS_LINE_PLANE_HPP_INCLUDED
