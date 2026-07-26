#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_PLANE_DISTANCE_PLANE_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_PLANE_DISTANCE_PLANE_SEGMENT_HPP_INCLUDED

#include "segment_plane_distance_segment_plane.hpp"

inline long double distance(const Plane3& plane, const Segment3& segment){
    return distance(segment, plane);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_PLANE_DISTANCE_PLANE_SEGMENT_HPP_INCLUDED
