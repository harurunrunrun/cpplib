#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_PLANE_LINE_PLANE_DISTANCE_LINE_PLANE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_PLANE_LINE_PLANE_DISTANCE_LINE_PLANE_HPP_INCLUDED

#include "../point/abs.hpp"
#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../point_set/line_plane/line_plane_closest_points_line_plane.hpp"

inline long double distance(const Line3& line, const Plane3& plane){
    const auto points = closest_points(line, plane);
    return abs(points.first - points.second);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_PLANE_LINE_PLANE_DISTANCE_LINE_PLANE_HPP_INCLUDED
