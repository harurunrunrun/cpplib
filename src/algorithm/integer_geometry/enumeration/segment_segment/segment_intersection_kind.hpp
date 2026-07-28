#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_ENUMERATION_SEGMENT_SEGMENT_SEGMENT_INTERSECTION_KIND_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_ENUMERATION_SEGMENT_SEGMENT_SEGMENT_INTERSECTION_KIND_HPP_INCLUDED

#include <algorithm>

#include "../../predicate/segment_point/on_segment.hpp"

namespace integer_geometry{

inline SegmentIntersectionKind segment_intersection_kind(
    const Segment& first,
    const Segment& second
){
    const int first_a = orientation(first.a, first.b, second.a);
    const int first_b = orientation(first.a, first.b, second.b);
    const int second_a = orientation(second.a, second.b, first.a);
    const int second_b = orientation(second.a, second.b, first.b);

    if(first_a == 0 && first_b == 0 && second_a == 0 && second_b == 0){
        const Point first_left = std::min(first.a, first.b);
        const Point first_right = std::max(first.a, first.b);
        const Point second_left = std::min(second.a, second.b);
        const Point second_right = std::max(second.a, second.b);
        const Point left = std::max(first_left, second_left);
        const Point right = std::min(first_right, second_right);
        if(right < left) return SegmentIntersectionKind::none;
        if(left == right) return SegmentIntersectionKind::touch;
        return SegmentIntersectionKind::overlap;
    }

    if(first_a * first_b < 0 && second_a * second_b < 0){
        return SegmentIntersectionKind::proper;
    }
    if((first_a == 0 && on_segment(first, second.a))
        || (first_b == 0 && on_segment(first, second.b))
        || (second_a == 0 && on_segment(second, first.a))
        || (second_b == 0 && on_segment(second, first.b))){
        return SegmentIntersectionKind::touch;
    }
    return SegmentIntersectionKind::none;
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_ENUMERATION_SEGMENT_SEGMENT_SEGMENT_INTERSECTION_KIND_HPP_INCLUDED
