#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_SEGMENT_SEGMENT_SEGMENTS_INTERSECT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_SEGMENT_SEGMENT_SEGMENTS_INTERSECT_HPP_INCLUDED

#include "../../enumeration/segment_segment/segment_intersection_kind.hpp"

namespace integer_geometry{

inline bool segments_intersect(const Segment& first, const Segment& second){
    return segment_intersection_kind(first, second)
        != SegmentIntersectionKind::none;
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_SEGMENT_SEGMENT_SEGMENTS_INTERSECT_HPP_INCLUDED
