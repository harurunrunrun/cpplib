#ifndef CPPLIB_SRC_INTEGER_GEOMETRY_2D_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_INTEGER_GEOMETRY_2D_INTERSECTION_HPP_INCLUDED

#include <algorithm>
#include <optional>
#include <vector>

#include "predicates.hpp"

namespace integer_geometry{

inline std::optional<Point> line_intersection(
    const Line& first,
    const Line& second
){
    const Vector first_direction = direction(first);
    const Vector second_direction = direction(second);
    const Rational denominator = cross(
        first_direction, second_direction
    );
    if(denominator.is_zero()) return std::nullopt;

    const Rational parameter =
        cross(second.a - first.a, second_direction) / denominator;
    return first.a + first_direction * parameter;
}

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

#endif  // CPPLIB_SRC_INTEGER_GEOMETRY_2D_INTERSECTION_HPP_INCLUDED
