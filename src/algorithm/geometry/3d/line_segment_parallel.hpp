#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_PARALLEL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_PARALLEL_HPP_INCLUDED

#include "geometry_primitives.hpp"
#include "line3_direction.hpp"
#include "parallel.hpp"
#include "segment3_direction.hpp"

inline bool parallel(const Line3& first, const Segment3& second){
    return parallel(line3_direction(first), segment3_direction(second));
}

inline bool parallel(const Segment3& first, const Line3& second){
    return parallel(second, first);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_PARALLEL_HPP_INCLUDED
