#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATES_HPP_INCLUDED

#include <algorithm>

#include "types.hpp"

namespace integer_geometry{

inline Wide dot(const Vector& first, const Vector& second){
    return detail::checked_add(
        detail::checked_multiply(first.x, second.x),
        detail::checked_multiply(first.y, second.y)
    );
}

inline Wide cross(const Vector& first, const Vector& second){
    return detail::checked_subtract(
        detail::checked_multiply(first.x, second.y),
        detail::checked_multiply(first.y, second.x)
    );
}

inline Wide cross(const Point& origin, const Point& first, const Point& second){
    return cross(vector_from(origin, first), vector_from(origin, second));
}

namespace detail{

struct SignedMagnitude{
    bool negative;
    UnsignedWide magnitude;
};

inline SignedMagnitude product_as_signed_magnitude(
    Wide first,
    Wide second
){
    const UnsignedWide first_magnitude = unsigned_magnitude(first);
    const UnsignedWide second_magnitude = unsigned_magnitude(second);
    UnsignedWide magnitude;
    if(__builtin_mul_overflow(
        first_magnitude, second_magnitude, &magnitude))[[unlikely]]{
        throw std::overflow_error(
            "integer geometry unsigned product overflow"
        );
    }
    return {
        magnitude != 0 && ((first < 0) != (second < 0)),
        magnitude,
    };
}

inline int signed_magnitude_sum_sign(
    SignedMagnitude first,
    SignedMagnitude second
){
    if(first.magnitude == 0) first.negative = false;
    if(second.magnitude == 0) second.negative = false;
    if(first.negative == second.negative){
        if(first.magnitude == 0 && second.magnitude == 0) return 0;
        return first.negative ? -1 : 1;
    }
    if(first.magnitude == second.magnitude) return 0;
    const bool first_dominates = first.magnitude > second.magnitude;
    const bool negative =
        first_dominates ? first.negative : second.negative;
    return negative ? -1 : 1;
}

inline int product_difference_sign(
    Wide first_left,
    Wide first_right,
    Wide second_left,
    Wide second_right
){
    const SignedMagnitude first =
        product_as_signed_magnitude(first_left, first_right);
    SignedMagnitude second =
        product_as_signed_magnitude(second_left, second_right);
    if(second.magnitude != 0) second.negative = !second.negative;
    return signed_magnitude_sum_sign(first, second);
}

inline int product_sum_sign(
    Wide first_left,
    Wide first_right,
    Wide second_left,
    Wide second_right
){
    return signed_magnitude_sum_sign(
        product_as_signed_magnitude(first_left, first_right),
        product_as_signed_magnitude(second_left, second_right)
    );
}

struct DoubleUnsignedWide{
    bool high;
    UnsignedWide low;
};

inline DoubleUnsignedWide absolute_product_difference(
    Wide first_left,
    Wide first_right,
    Wide second_left,
    Wide second_right
){
    const SignedMagnitude first =
        product_as_signed_magnitude(first_left, first_right);
    const SignedMagnitude second =
        product_as_signed_magnitude(second_left, second_right);
    if(first.negative == second.negative){
        return {
            false,
            first.magnitude >= second.magnitude
                ? first.magnitude - second.magnitude
                : second.magnitude - first.magnitude,
        };
    }
    const UnsignedWide low = first.magnitude + second.magnitude;
    return {low < first.magnitude, low};
}

inline bool double_unsigned_less_equal(
    const DoubleUnsignedWide& first,
    const DoubleUnsignedWide& second
){
    if(first.high != second.high) return !first.high;
    return first.low <= second.low;
}

}  // namespace detail

inline int orientation(
    const Point& first,
    const Point& second,
    const Point& third
){
    const Vector first_direction = vector_from(first, second);
    const Vector second_direction = vector_from(first, third);
    return detail::product_difference_sign(
        first_direction.x, second_direction.y,
        first_direction.y, second_direction.x);
}

inline int ccw(
    const Point& first,
    const Point& second,
    const Point& third
){
    return orientation(first, second, third);
}

inline bool on_segment(const Segment& segment, const Point& point){
    if(orientation(segment.a, segment.b, point) != 0) return false;
    return std::min(segment.a.x, segment.b.x) <= point.x
        && point.x <= std::max(segment.a.x, segment.b.x)
        && std::min(segment.a.y, segment.b.y) <= point.y
        && point.y <= std::max(segment.a.y, segment.b.y);
}

inline bool parallel(const Segment& first, const Segment& second){
    const Vector first_direction = vector_from(first.a, first.b);
    const Vector second_direction = vector_from(second.a, second.b);
    return detail::product_difference_sign(
        first_direction.x, second_direction.y,
        first_direction.y, second_direction.x
    ) == 0;
}

inline bool orthogonal(const Segment& first, const Segment& second){
    const Vector first_direction = vector_from(first.a, first.b);
    const Vector second_direction = vector_from(second.a, second.b);
    return detail::product_sum_sign(
        first_direction.x, second_direction.x,
        first_direction.y, second_direction.y
    ) == 0;
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
        Point first_left = std::min(first.a, first.b);
        Point first_right = std::max(first.a, first.b);
        Point second_left = std::min(second.a, second.b);
        Point second_right = std::max(second.a, second.b);
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

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATES_HPP_INCLUDED
