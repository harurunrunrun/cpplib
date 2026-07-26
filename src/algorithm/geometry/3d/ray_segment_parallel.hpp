#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_PARALLEL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_PARALLEL_HPP_INCLUDED

#include "geometry_primitives.hpp"
#include "parallel.hpp"
#include "ray3_direction.hpp"
#include "segment3_direction.hpp"

inline bool parallel(const Ray3& first, const Segment3& second){
    return parallel(ray3_direction(first), segment3_direction(second));
}

inline bool parallel(const Segment3& first, const Ray3& second){
    return parallel(second, first);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_SEGMENT_PARALLEL_HPP_INCLUDED
