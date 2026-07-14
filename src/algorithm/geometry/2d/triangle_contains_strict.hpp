#pragma once

#include "cross.hpp"

inline bool triangle_contains_strict(
    const Point& first,
    const Point& second,
    const Point& third,
    const Point& point
){
    const int first_side = geometry_sign(cross(second - first, point - first));
    const int second_side = geometry_sign(cross(third - second, point - second));
    const int third_side = geometry_sign(cross(first - third, point - third));
    return first_side != 0 &&
        first_side == second_side && second_side == third_side;
}
