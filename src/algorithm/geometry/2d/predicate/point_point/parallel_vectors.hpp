#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_POINT_POINT_PARALLEL_VECTORS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_POINT_POINT_PARALLEL_VECTORS_HPP_INCLUDED

#include <cmath>

#include "../../scalar/point/abs.hpp"
#include "../../scalar/point_point/cross.hpp"

inline bool parallel(const Point& left, const Point& right){
    const long double left_length = abs(left);
    const long double right_length = abs(right);
    if(geometry_sign(left_length) == 0 ||
       geometry_sign(right_length) == 0){
        return true;
    }
    const long double roundoff =
        std::abs(left.x * right.y) + std::abs(left.y * right.x);
    return geometry_scaled_sign(
        cross(left, right), left_length * right_length, roundoff
    ) == 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_POINT_POINT_PARALLEL_VECTORS_HPP_INCLUDED
