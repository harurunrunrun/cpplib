#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SEGMENT_SEGMENT_SEGMENT_SEGMENT_PARALLEL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SEGMENT_SEGMENT_SEGMENT_SEGMENT_PARALLEL_HPP_INCLUDED

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../point_point/parallel.hpp"
#include "../../point/segment/segment3_direction.hpp"

inline bool parallel(const Segment3& first, const Segment3& second){
    return parallel(segment3_direction(first), segment3_direction(second));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SEGMENT_SEGMENT_SEGMENT_SEGMENT_PARALLEL_HPP_INCLUDED
