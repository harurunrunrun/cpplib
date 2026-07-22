#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_INTERSECT_CIRCLE_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_INTERSECT_CIRCLE_SEGMENT_HPP_INCLUDED

#include <algorithm>
#include <cmath>

#include "../scalar/dot.hpp"
#include "../scalar/norm.hpp"
#include "validate_circle.hpp"

inline bool intersect_circle_segment(
    const Circle& circle,
    const Segment& segment
){
    validate_circle(circle);
    const Point direction = segment.b - segment.a;
    const long double direction_length = std::hypot(
        direction.x, direction.y
    );
    const bool point_segment = geometry_sign(direction_length) == 0;
    const long double direction_squared =
        direction_length * direction_length;
    long double parameter = 0.0L;
    if(!point_segment){
        parameter = std::clamp(
            dot(circle.center - segment.a, direction) / direction_squared,
            0.0L,
            1.0L
        );
    }
    const long double minimum_squared = norm(
        segment.a + direction * parameter - circle.center
    );
    const long double maximum_squared = point_segment
        ? norm(segment.a - circle.center)
        : std::max(
            norm(segment.a - circle.center),
            norm(segment.b - circle.center)
        );
    const long double radius_squared = circle.radius * circle.radius;
    return circle_numeric_detail::scaled_sign(
        minimum_squared - radius_squared,
        minimum_squared + radius_squared
    ) <= 0 && circle_numeric_detail::scaled_sign(
        radius_squared - maximum_squared,
        radius_squared + maximum_squared
    ) <= 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_INTERSECT_CIRCLE_SEGMENT_HPP_INCLUDED
