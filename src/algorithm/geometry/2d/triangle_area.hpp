#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_AREA_HPP_INCLUDED

#include <cmath>

#include "triangle_signed_area.hpp"

inline long double triangle_area(
    const Point& first,
    const Point& second,
    const Point& third
){
    return std::abs(triangle_signed_area(first, second, third));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_AREA_HPP_INCLUDED
