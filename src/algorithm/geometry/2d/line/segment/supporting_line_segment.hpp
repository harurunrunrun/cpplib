#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_SEGMENT_SUPPORTING_LINE_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_SEGMENT_SUPPORTING_LINE_SEGMENT_HPP_INCLUDED

#include "../../type/definition/line.hpp"
#include "../../type/definition/segment_type.hpp"

inline Line supporting_line(const Segment& segment){
    return {segment.a, segment.b};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_SEGMENT_SUPPORTING_LINE_SEGMENT_HPP_INCLUDED
