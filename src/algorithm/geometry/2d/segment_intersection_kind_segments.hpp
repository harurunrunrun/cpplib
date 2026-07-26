#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SEGMENT_INTERSECTION_KIND_SEGMENTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SEGMENT_INTERSECTION_KIND_SEGMENTS_HPP_INCLUDED

#include <optional>

#include "intersect_segment_segment.hpp"
#include "segment_intersection_kind_result.hpp"
#include "segment_overlap.hpp"

inline SegmentIntersectionKind segment_intersection_kind(
    const Segment& first,
    const Segment& second
){
    if(!intersect(first, second)) return SEGMENT_INTERSECTION_NONE;
    const std::optional<Segment> overlap = segment_overlap(first, second);
    if(!overlap.has_value()) return SEGMENT_INTERSECTION_POINT;
    return overlap->a == overlap->b
        ? SEGMENT_INTERSECTION_POINT
        : SEGMENT_INTERSECTION_SEGMENT;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SEGMENT_INTERSECTION_KIND_SEGMENTS_HPP_INCLUDED
