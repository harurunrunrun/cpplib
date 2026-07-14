#pragma once

#include <cmath>

#include "types.hpp"

inline long double abs(const Point& point){
    return std::hypot(point.x, point.y);
}
