#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SEGMENT_PLANE_SEGMENT_PLANE_DISTANCE_SEGMENT_PLANE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SEGMENT_PLANE_SEGMENT_PLANE_DISTANCE_SEGMENT_PLANE_HPP_INCLUDED

#include "../point/abs.hpp"
#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../point_set/segment_plane/segment_plane_closest_points_segment_plane.hpp"

inline long double distance(const Segment3& segment, const Plane3& plane){
    const auto points = closest_points(segment, plane);
    return abs(points.first - points.second);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SEGMENT_PLANE_SEGMENT_PLANE_DISTANCE_SEGMENT_PLANE_HPP_INCLUDED
