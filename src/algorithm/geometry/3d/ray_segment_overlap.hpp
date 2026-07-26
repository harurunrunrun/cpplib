#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_OVERLAP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_OVERLAP_HPP_INCLUDED

#include <variant>

#include "geometry_primitives.hpp"
#include "ray_segment_intersection.hpp"

inline bool overlap(const Ray3& first, const Segment3& second){
    return std::holds_alternative<Segment3>(
        ray_segment_intersection(first, second)
    );
}

inline bool overlap(const Segment3& first, const Ray3& second){
    return overlap(second, first);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_OVERLAP_HPP_INCLUDED
