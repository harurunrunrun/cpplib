#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_CROSS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_CROSS_HPP_INCLUDED

#include "../core/types.hpp"

inline long double cross(const Point& left, const Point& right){
    return left.x * right.y - left.y * right.x;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_CROSS_HPP_INCLUDED
