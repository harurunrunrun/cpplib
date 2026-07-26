#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_SECTOR_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_SECTOR_AREA_HPP_INCLUDED

#include <cmath>
#include <stdexcept>

#include "validate_circle.hpp"

inline long double circle_sector_area(
    const Circle& circle,
    long double central_angle
){
    validate_circle(circle);
    if(!std::isfinite(central_angle) || central_angle < 0.0L){
        throw std::invalid_argument("central angle must be finite and nonnegative");
    }
    return circle.radius * circle.radius * central_angle / 2.0L;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_CIRCLE_SECTOR_AREA_HPP_INCLUDED
