#pragma once

#include <cmath>

#include "triangle_signed_area.hpp"

inline long double triangle_area(
    const Point& first,
    const Point& second,
    const Point& third
){
    return std::abs(triangle_signed_area(first, second, third));
}
