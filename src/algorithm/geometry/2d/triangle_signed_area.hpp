#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_SIGNED_AREA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_SIGNED_AREA_HPP_INCLUDED

#include "cross.hpp"

inline long double triangle_signed_area(
    const Point& first,
    const Point& second,
    const Point& third
){
    return cross(second - first, third - first) / 2.0L;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_TRIANGLE_SIGNED_AREA_HPP_INCLUDED
