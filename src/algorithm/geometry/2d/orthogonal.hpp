#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "dot.hpp"

inline bool orthogonal(const Point& left, const Point& right){
    return geometry_sign(dot(left, right)) == 0;
}

inline bool orthogonal(const Line& left, const Line& right){
    if(geometry_sign(abs(left.b - left.a)) == 0 ||
        geometry_sign(abs(right.b - right.a)) == 0){
        throw std::invalid_argument("degenerate line");
    }
    return orthogonal(left.b - left.a, right.b - right.a);
}
