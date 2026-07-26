#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_PARALLEL_SEGMENT_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_PARALLEL_SEGMENT_LINE_HPP_INCLUDED

#include "line_segment_parallel_line_segment.hpp"

inline bool parallel(const Segment3& first, const Line3& second){
    return parallel(second, first);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_PARALLEL_SEGMENT_LINE_HPP_INCLUDED