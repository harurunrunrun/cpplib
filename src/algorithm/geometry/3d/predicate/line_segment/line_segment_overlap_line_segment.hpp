#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_SEGMENT_LINE_SEGMENT_OVERLAP_LINE_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_SEGMENT_LINE_SEGMENT_OVERLAP_LINE_SEGMENT_HPP_INCLUDED

#include <variant>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../result/line_segment/line_segment_intersection.hpp"

inline bool overlap(const Line3& first, const Segment3& second){
    return std::holds_alternative<Segment3>(
        line_segment_intersection(first, second)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_SEGMENT_LINE_SEGMENT_OVERLAP_LINE_SEGMENT_HPP_INCLUDED
