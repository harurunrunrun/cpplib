#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_INTERSECT_CIRCLE_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_INTERSECT_CIRCLE_LINE_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "../scalar/cross.hpp"
#include "validate_circle.hpp"

inline bool intersect_circle_line(const Circle& circle, const Line& line){
    validate_circle(circle);
    const Point direction = line.b - line.a;
    const long double direction_length = std::hypot(
        direction.x, direction.y
    );
    if(geometry_sign(direction_length) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    const long double cross_value = cross(
        direction, circle.center - line.a
    );
    const long double left = cross_value * cross_value;
    const long double right =
        circle.radius * circle.radius
        * direction_length * direction_length;
    return circle_numeric_detail::scaled_sign(
        left - right, left + right
    ) <= 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_INTERSECT_CIRCLE_LINE_HPP_INCLUDED
