#pragma once

#include "types.hpp"

inline long double dot(const Point& left, const Point& right){
    return left.x * right.x + left.y * right.y;
}
