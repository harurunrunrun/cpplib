#pragma once

#include "abs.hpp"
#include "cross.hpp"
#include "dot.hpp"

inline bool on_segment(const Segment& segment, const Point& point){
    const Point direction = segment.b - segment.a;
    const long double length = abs(direction);
    if(geometry_sign(length) == 0) return point == segment.a;
    const Point relative = point - segment.a;
    const long double cross_roundoff =
        std::abs(direction.x * relative.y) + std::abs(direction.y * relative.x);
    if(geometry_scaled_sign(
        cross(direction, relative), length, cross_roundoff
    ) != 0){
        return false;
    }
    const long double projection = dot(relative, direction);
    const long double squared_length = dot(direction, direction);
    const long double dot_roundoff =
        std::abs(direction.x * relative.x) + std::abs(direction.y * relative.y);
    return geometry_scaled_sign(
        projection, length, dot_roundoff
    ) >= 0 && geometry_scaled_sign(
        projection - squared_length,
        length,
        dot_roundoff + squared_length
    ) <= 0;
}
