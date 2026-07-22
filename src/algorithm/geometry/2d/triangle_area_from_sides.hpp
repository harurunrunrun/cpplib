#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_AREA_FROM_SIDES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_AREA_FROM_SIDES_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

#include "geometry_sign.hpp"

inline long double triangle_area_from_sides(
    long double first,
    long double second,
    long double third
){
    if(!std::isfinite(first) || !std::isfinite(second) ||
       !std::isfinite(third) || first < 0 || second < 0 || third < 0){
        throw std::invalid_argument("triangle sides must be finite and nonnegative");
    }
    std::array<long double, 3> sides{first, second, third};
    std::sort(sides.begin(), sides.end(), std::greater<>());
    const long double largest = sides[0];
    const long double middle = sides[1];
    const long double smallest = sides[2];
    if(geometry_sign(largest - middle - smallest) > 0){
        throw std::invalid_argument("triangle inequality is not satisfied");
    }
    const long double product =
        (largest + (middle + smallest)) *
        (smallest - (largest - middle)) *
        (smallest + (largest - middle)) *
        (largest + (middle - smallest));
    return std::sqrt(std::max(0.0L, product)) / 4.0L;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_AREA_FROM_SIDES_HPP_INCLUDED
