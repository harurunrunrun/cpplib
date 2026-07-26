#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_PLANE_CLOSEST_POINTS_PLANE_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_PLANE_CLOSEST_POINTS_PLANE_SEGMENT_HPP_INCLUDED

#include <utility>

#include "segment_plane_closest_points_segment_plane.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Plane3& plane,
    const Segment3& segment
){
    const auto result = closest_points(segment, plane);
    return {result.second, result.first};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_PLANE_CLOSEST_POINTS_PLANE_SEGMENT_HPP_INCLUDED
