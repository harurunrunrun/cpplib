#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_RATIONAL_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_RATIONAL_INTERSECTION_HPP_INCLUDED

#include <optional>

#include "predicates.hpp"

namespace integer_geometry{

inline RationalPoint rational_point(const Point& point){
    return {
        Rational(static_cast<Wide>(point.x), 1),
        Rational(static_cast<Wide>(point.y), 1),
    };
}

namespace detail{

inline Rational affine_rational_coordinate(
    Coordinate origin,
    Wide direction,
    const Rational& parameter
){
    if(direction == 0) return Rational(origin, 1);
    const UnsignedWide divisor = gcd_wide(
        unsigned_magnitude(direction),
        static_cast<UnsignedWide>(parameter.denominator)
    );
    const Wide signed_divisor = static_cast<Wide>(divisor);
    const Wide reduced_direction = direction / signed_divisor;
    const Wide reduced_denominator =
        parameter.denominator / signed_divisor;
    const Wide numerator = checked_add(
        checked_multiply(static_cast<Wide>(origin), reduced_denominator),
        checked_multiply(reduced_direction, parameter.numerator)
    );
    return Rational(numerator, reduced_denominator);
}

}  // namespace detail

inline std::optional<RationalPoint> line_intersection(
    const Line& first,
    const Line& second
){
    const Vector first_direction = vector_from(first.a, first.b);
    const Vector second_direction = vector_from(second.a, second.b);
    const Wide denominator = cross(first_direction, second_direction);
    if(denominator == 0) return std::nullopt;

    const Wide parameter_numerator = cross(
        vector_from(first.a, second.a),
        second_direction
    );
    const Rational parameter(parameter_numerator, denominator);
    return RationalPoint{
        detail::affine_rational_coordinate(
            first.a.x, first_direction.x, parameter),
        detail::affine_rational_coordinate(
            first.a.y, first_direction.y, parameter),
    };
}

inline std::optional<RationalPoint> segment_intersection_point(
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
        if(on_segment(first, second.a)) return rational_point(second.a);
        if(on_segment(first, second.b)) return rational_point(second.b);
        if(on_segment(second, first.a)) return rational_point(first.a);
        return rational_point(first.b);
    }
    return line_intersection({first.a, first.b}, {second.a, second.b});
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_RATIONAL_INTERSECTION_HPP_INCLUDED
