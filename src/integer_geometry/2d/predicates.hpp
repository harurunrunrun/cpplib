#ifndef CPPLIB_SRC_INTEGER_GEOMETRY_2D_PREDICATES_HPP_INCLUDED
#define CPPLIB_SRC_INTEGER_GEOMETRY_2D_PREDICATES_HPP_INCLUDED

#include <algorithm>

#include "types.hpp"

namespace integer_geometry{

inline Rational dot(const Vector& first, const Vector& second){
    return first.x * second.x + first.y * second.y;
}

inline Rational cross(const Vector& first, const Vector& second){
    return first.x * second.y - first.y * second.x;
}

inline Rational cross(
    const Point& origin,
    const Point& first,
    const Point& second
){
    return cross(first - origin, second - origin);
}

inline int orientation(
    const Point& first,
    const Point& second,
    const Point& third
){
    return cross(first, second, third).sign();
}

inline int ccw(
    const Point& first,
    const Point& second,
    const Point& third
){
    return orientation(first, second, third);
}

inline bool on_line(const Line& line, const Point& point){
    return orientation(line.a, line.b, point) == 0;
}

inline bool on_segment(const Segment& segment, const Point& point){
    if(orientation(segment.a, segment.b, point) != 0) return false;
    return std::min(segment.a.x, segment.b.x) <= point.x
        && point.x <= std::max(segment.a.x, segment.b.x)
        && std::min(segment.a.y, segment.b.y) <= point.y
        && point.y <= std::max(segment.a.y, segment.b.y);
}

inline bool parallel(const Vector& first, const Vector& second){
    return cross(first, second).is_zero();
}

inline bool orthogonal(const Vector& first, const Vector& second){
    return dot(first, second).is_zero();
}

inline bool parallel(const Line& first, const Line& second){
    return parallel(direction(first), direction(second));
}

inline bool orthogonal(const Line& first, const Line& second){
    return orthogonal(direction(first), direction(second));
}

inline bool parallel(const Segment& first, const Segment& second){
    return parallel(direction(first), direction(second));
}

inline bool orthogonal(const Segment& first, const Segment& second){
    return orthogonal(direction(first), direction(second));
}

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

inline bool segments_intersect(const Segment& first, const Segment& second){
    return segment_intersection_kind(first, second)
        != SegmentIntersectionKind::none;
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_INTEGER_GEOMETRY_2D_PREDICATES_HPP_INCLUDED
