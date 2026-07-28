#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SEGMENT_SEGMENT_SEGMENT_SEGMENT_OVERLAP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SEGMENT_SEGMENT_SEGMENT_SEGMENT_OVERLAP_HPP_INCLUDED

#include <variant>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../result/segment_segment/segment_segment_intersection.hpp"

inline bool overlap(const Segment3& first, const Segment3& second){
    return std::holds_alternative<Segment3>(
        segment_segment_intersection(first, second)
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SEGMENT_SEGMENT_SEGMENT_SEGMENT_OVERLAP_HPP_INCLUDED
