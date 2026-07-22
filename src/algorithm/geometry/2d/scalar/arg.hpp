#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_ARG_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_ARG_HPP_INCLUDED

#include <cmath>

#include "../core/types.hpp"

inline long double arg(const Point& point){
    return std::atan2(point.y, point.x);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_ARG_HPP_INCLUDED
