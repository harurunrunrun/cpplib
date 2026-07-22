#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DOT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DOT_HPP_INCLUDED

#include "types.hpp"

inline long double dot(const Point& left, const Point& right){
    return left.x * right.x + left.y * right.y;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_DOT_HPP_INCLUDED
