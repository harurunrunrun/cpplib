#pragma once

#include "types.hpp"

inline long double cross(const Point& left, const Point& right){
    return left.x * right.y - left.y * right.x;
}
