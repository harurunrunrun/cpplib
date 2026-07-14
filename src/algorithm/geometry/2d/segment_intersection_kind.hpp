#pragma once

#include "intersect.hpp"
#include "segment_overlap.hpp"

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
