#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "cross.hpp"

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

inline bool parallel(const Line& left, const Line& right){
    if(geometry_sign(abs(left.b - left.a)) == 0 ||
        geometry_sign(abs(right.b - right.a)) == 0){
        throw std::invalid_argument("degenerate line");
    }
    return parallel(left.b - left.a, right.b - right.a);
}
