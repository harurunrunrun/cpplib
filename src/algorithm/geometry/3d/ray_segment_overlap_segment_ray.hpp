#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_OVERLAP_SEGMENT_RAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_OVERLAP_SEGMENT_RAY_HPP_INCLUDED

#include "ray_segment_overlap_ray_segment.hpp"

inline bool overlap(const Segment3& first, const Ray3& second){
    return overlap(second, first);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_OVERLAP_SEGMENT_RAY_HPP_INCLUDED