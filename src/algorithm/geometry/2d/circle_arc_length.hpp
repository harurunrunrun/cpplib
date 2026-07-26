#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_ARC_LENGTH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_ARC_LENGTH_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "validate_circle.hpp"

inline long double circle_arc_length(
    const Circle& circle,
    long double central_angle
){
    validate_circle(circle);
    if(!std::isfinite(central_angle) || central_angle < 0.0L){
        throw std::invalid_argument("central angle must be finite and nonnegative");
    }
    return circle.radius * central_angle;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_ARC_LENGTH_HPP_INCLUDED
