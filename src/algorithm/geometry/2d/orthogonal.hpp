#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ORTHOGONAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ORTHOGONAL_HPP_INCLUDED

#include <stdexcept>

#include "abs.hpp"
#include "dot.hpp"

inline bool orthogonal(const Point& left, const Point& right){
    const long double left_length = abs(left);
    const long double right_length = abs(right);
    if(geometry_sign(left_length) == 0 ||
       geometry_sign(right_length) == 0){
        return true;
    }
    const long double roundoff =
        std::abs(left.x * right.x) + std::abs(left.y * right.y);
    return geometry_scaled_sign(
        dot(left, right), left_length * right_length, roundoff
    ) == 0;
}

inline bool orthogonal(const Line& left, const Line& right){
    if(geometry_sign(abs(left.b - left.a)) == 0 ||
        geometry_sign(abs(right.b - right.a)) == 0){
        throw std::invalid_argument("degenerate line");
    }
    return orthogonal(left.b - left.a, right.b - right.a);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ORTHOGONAL_HPP_INCLUDED
