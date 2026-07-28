#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_POINT_CROSS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_POINT_CROSS_HPP_INCLUDED

#include "../../aggregate/all/types.hpp"

inline long double cross(const Point& left, const Point& right){
    return left.x * right.y - left.y * right.x;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SCALAR_POINT_POINT_CROSS_HPP_INCLUDED
