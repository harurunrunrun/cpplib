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
    (void)line3_direction(line);
    (void)plane3_unit_normal(plane);
    const auto intersection = geometry3d_line_plane_detail::intersection_data(
        line.a, line.b, plane
    );
    if(intersection) return {intersection->point, intersection->point};
    return {line.a, projection(plane, line.a)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_POINT_SET_LINE_PLANE_LINE_PLANE_CLOSEST_POINTS_LINE_PLANE_HPP_INCLUDED
