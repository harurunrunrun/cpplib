#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SEGMENT_PLANE_DISTANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SEGMENT_PLANE_DISTANCE_HPP_INCLUDED

#include "abs.hpp"
#include "../core/geometry_primitives.hpp"
#include "../point_collection/segment_plane_closest_points.hpp"

inline long double distance(const Segment3& segment, const Plane3& plane){
    const auto points = closest_points(segment, plane);
    return abs(points.first - points.second);
}

inline long double distance(const Plane3& plane, const Segment3& segment){
    return distance(segment, plane);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SEGMENT_PLANE_DISTANCE_HPP_INCLUDED
