#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_SEGMENT_LINE_SEGMENT_PARALLEL_LINE_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_SEGMENT_LINE_SEGMENT_PARALLEL_LINE_SEGMENT_HPP_INCLUDED

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../point/line/line3_direction.hpp"
#include "../point_point/parallel.hpp"
#include "../../point/segment/segment3_direction.hpp"

inline bool parallel(const Line3& first, const Segment3& second){
    return parallel(line3_direction(first), segment3_direction(second));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_SEGMENT_LINE_SEGMENT_PARALLEL_LINE_SEGMENT_HPP_INCLUDED
