#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_PLANE_DISTANCE_SEGMENT_PLANE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_PLANE_DISTANCE_SEGMENT_PLANE_HPP_INCLUDED

#include "abs.hpp"
#include "geometry_primitives.hpp"
#include "segment_plane_closest_points_segment_plane.hpp"

inline long double distance(const Segment3& segment, const Plane3& plane){
    const auto points = closest_points(segment, plane);
    return abs(points.first - points.second);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_PLANE_DISTANCE_SEGMENT_PLANE_HPP_INCLUDED
