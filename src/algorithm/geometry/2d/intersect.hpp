#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "cross.hpp"
#include "on_segment.hpp"

inline bool intersect(const Segment& first, const Segment& second){
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
    if(first_a == 0 && on_segment(first, second.a)) return true;
    if(first_b == 0 && on_segment(first, second.b)) return true;
    if(second_a == 0 && on_segment(second, first.a)) return true;
    if(second_b == 0 && on_segment(second, first.b)) return true;
    return first_a * first_b < 0 && second_a * second_b < 0;
}

inline bool intersect(const Line& line, const Point& point){
    if(geometry_sign(abs(line.b - line.a)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    return geometry_sign(cross(line.b - line.a, point - line.a)) == 0;
}
