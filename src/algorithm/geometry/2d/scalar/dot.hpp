#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_DOT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_DOT_HPP_INCLUDED

#include "../core/types.hpp"

inline long double dot(const Point& left, const Point& right){
    return left.x * right.x + left.y * right.y;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_DOT_HPP_INCLUDED
