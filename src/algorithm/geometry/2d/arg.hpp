#pragma once

#include <cmath>

#include "types.hpp"

inline long double arg(const Point& point){
    return std::atan2(point.y, point.x);
}
