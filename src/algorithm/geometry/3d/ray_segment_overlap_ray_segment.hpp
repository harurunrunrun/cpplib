#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_OVERLAP_RAY_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_OVERLAP_RAY_SEGMENT_HPP_INCLUDED

#include <variant>

#include "geometry_primitives.hpp"
#include "ray_segment_intersection.hpp"

inline bool overlap(const Ray3& first, const Segment3& second){
    return std::holds_alternative<Segment3>(
        ray_segment_intersection(first, second)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_OVERLAP_RAY_SEGMENT_HPP_INCLUDED