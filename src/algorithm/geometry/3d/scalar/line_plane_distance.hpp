#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_PLANE_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_PLANE_DISTANCE_HPP_INCLUDED

#include "abs.hpp"
#include "../core/geometry_primitives.hpp"
#include "../point_collection/line_plane_closest_points.hpp"

inline long double distance(const Line3& line, const Plane3& plane){
    const auto points = closest_points(line, plane);
    return abs(points.first - points.second);
}

inline long double distance(const Plane3& plane, const Line3& line){
    return distance(line, plane);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_LINE_PLANE_DISTANCE_HPP_INCLUDED
