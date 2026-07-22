#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_ABS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_ABS_HPP_INCLUDED

#include <cmath>

#include "../core/types.hpp"

inline long double abs(const Point& point){
    return std::hypot(point.x, point.y);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_ABS_HPP_INCLUDED
