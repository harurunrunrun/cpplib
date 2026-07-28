#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_SET_SEGMENT_SEGMENT_SEGMENT_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_SET_SEGMENT_SEGMENT_SEGMENT_INTERSECTION_HPP_INCLUDED

#include <algorithm>
#include <vector>

#include "../../result/segment_segment/segment_intersection_point.hpp"

namespace integer_geometry{

inline std::vector<Point> segment_intersection(
    const Segment& first,
    const Segment& second
){
    const SegmentIntersectionKind kind =
        segment_intersection_kind(first, second);
    if(kind == SegmentIntersectionKind::none) return {};
    if(kind != SegmentIntersectionKind::overlap){
        return {*segment_intersection_point(first, second)};
    }
    const Point left = std::max(
        std::min(first.a, first.b),
        std::min(second.a, second.b)
    );
    const Point right = std::min(
        std::max(first.a, first.b),
        std::max(second.a, second.b)
    );
    if(left == right) return {left};
    return {left, right};
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_POINT_SET_SEGMENT_SEGMENT_SEGMENT_INTERSECTION_HPP_INCLUDED
