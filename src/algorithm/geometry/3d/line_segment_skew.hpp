#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_SKEW_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_SKEW_HPP_INCLUDED

#include "geometry_primitives.hpp"
#include "line_line_skew.hpp"
#include "segment3_direction.hpp"

inline bool skew(const Line3& first, const Segment3& second){
    (void)segment3_direction(second);
    return skew(first, Line3{second.a, second.b});
}

inline bool skew(const Segment3& first, const Line3& second){
    return skew(second, first);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_SEGMENT_SKEW_HPP_INCLUDED
