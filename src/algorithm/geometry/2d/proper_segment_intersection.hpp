#pragma once

#include "cross.hpp"

inline bool proper_segment_intersection(
    const Segment& first,
    const Segment& second
){
    const int first_a = geometry_sign(cross(
        first.b - first.a, second.a - first.a
    ));
    const int first_b = geometry_sign(cross(
        first.b - first.a, second.b - first.a
    ));
    const int second_a = geometry_sign(cross(
        second.b - second.a, first.a - second.a
    ));
    const int second_b = geometry_sign(cross(
        second.b - second.a, first.b - second.a
    ));
    return first_a * first_b < 0 && second_a * second_b < 0;
}
