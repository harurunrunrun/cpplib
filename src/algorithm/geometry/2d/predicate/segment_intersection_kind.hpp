#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_SEGMENT_INTERSECTION_KIND_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_SEGMENT_INTERSECTION_KIND_HPP_INCLUDED

#include "intersect.hpp"
#include "../shape/segment_overlap.hpp"

enum SegmentIntersectionKind{
    SEGMENT_INTERSECTION_NONE,
    SEGMENT_INTERSECTION_POINT,
    SEGMENT_INTERSECTION_SEGMENT,
};

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_SEGMENT_INTERSECTION_KIND_HPP_INCLUDED
