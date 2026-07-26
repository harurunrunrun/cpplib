#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_SEGMENT_PARALLEL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_SEGMENT_PARALLEL_HPP_INCLUDED

#include "geometry_primitives.hpp"
#include "parallel.hpp"
#include "segment3_direction.hpp"

inline bool parallel(const Segment3& first, const Segment3& second){
    return parallel(segment3_direction(first), segment3_direction(second));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_SEGMENT_PARALLEL_HPP_INCLUDED
