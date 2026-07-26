#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_PLANE_CLOSEST_POINTS_PLANE_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_PLANE_CLOSEST_POINTS_PLANE_LINE_HPP_INCLUDED

#include <utility>

#include "line_plane_closest_points_line_plane.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Plane3& plane,
    const Line3& line
){
    const auto result = closest_points(line, plane);
    return {result.second, result.first};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_PLANE_CLOSEST_POINTS_PLANE_LINE_HPP_INCLUDED
