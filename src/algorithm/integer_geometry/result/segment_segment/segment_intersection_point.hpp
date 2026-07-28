#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_RESULT_SEGMENT_SEGMENT_SEGMENT_INTERSECTION_POINT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_RESULT_SEGMENT_SEGMENT_SEGMENT_INTERSECTION_POINT_HPP_INCLUDED

#include <optional>

#include "../../enumeration/segment_segment/segment_intersection_kind.hpp"
#include "../line_line/line_intersection.hpp"

namespace integer_geometry{

inline std::optional<Point> segment_intersection_point(
    const Segment& first,
    const Segment& second
){
    const SegmentIntersectionKind kind =
        segment_intersection_kind(first, second);
    if(kind == SegmentIntersectionKind::none
        || kind == SegmentIntersectionKind::overlap){
        return std::nullopt;
    }
    if(kind == SegmentIntersectionKind::touch){
        if(on_segment(first, second.a)) return second.a;
        if(on_segment(first, second.b)) return second.b;
        if(on_segment(second, first.a)) return first.a;
        return first.b;
    }
    return line_intersection(
        Line{first.a, first.b}, Line{second.a, second.b}
    );
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_RESULT_SEGMENT_SEGMENT_SEGMENT_INTERSECTION_POINT_HPP_INCLUDED
