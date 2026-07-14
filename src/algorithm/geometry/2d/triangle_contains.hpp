#pragma once

#include "on_segment.hpp"
#include "triangle_signed_area.hpp"

inline bool triangle_contains(
    const Point& first,
    const Point& second,
    const Point& third,
    const Point& point
){
    if(geometry_sign(cross(second - first, third - first)) == 0){
        return on_segment(Segment{first, second}, point) ||
            on_segment(Segment{second, third}, point) ||
            on_segment(Segment{third, first}, point);
    }
    const int first_side = geometry_sign(cross(second - first, point - first));
    const int second_side = geometry_sign(cross(third - second, point - second));
    const int third_side = geometry_sign(cross(first - third, point - third));
    const bool nonnegative = first_side >= 0 && second_side >= 0 && third_side >= 0;
    const bool nonpositive = first_side <= 0 && second_side <= 0 && third_side <= 0;
    return nonnegative || nonpositive;
}
