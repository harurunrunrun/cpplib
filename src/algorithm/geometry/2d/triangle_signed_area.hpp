#pragma once

#include "cross.hpp"

inline long double triangle_signed_area(
    const Point& first,
    const Point& second,
    const Point& third
){
    return cross(second - first, third - first) / 2.0L;
}
