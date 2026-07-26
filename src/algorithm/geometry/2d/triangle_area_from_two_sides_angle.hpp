#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_AREA_FROM_TWO_SIDES_ANGLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_AREA_FROM_TWO_SIDES_ANGLE_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

inline long double triangle_area_from_two_sides_angle(
    long double first,
    long double second,
    long double angle
){
    if(!std::isfinite(first) || !std::isfinite(second) ||
       !std::isfinite(angle) || first < 0 || second < 0){
        throw std::invalid_argument("sides and angle must be finite and sides nonnegative");
    }
    return std::abs(first * second * std::sin(angle)) / 2.0L;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_AREA_FROM_TWO_SIDES_ANGLE_HPP_INCLUDED
