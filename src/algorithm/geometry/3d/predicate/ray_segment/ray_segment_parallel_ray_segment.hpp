#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_RAY_SEGMENT_RAY_SEGMENT_PARALLEL_RAY_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_RAY_SEGMENT_RAY_SEGMENT_PARALLEL_RAY_SEGMENT_HPP_INCLUDED

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../point_point/parallel.hpp"
#include "../../point/ray/ray3_direction.hpp"
#include "../../point/segment/segment3_direction.hpp"

inline bool parallel(const Ray3& first, const Segment3& second){
    return parallel(ray3_direction(first), segment3_direction(second));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_RAY_SEGMENT_RAY_SEGMENT_PARALLEL_RAY_SEGMENT_HPP_INCLUDED
