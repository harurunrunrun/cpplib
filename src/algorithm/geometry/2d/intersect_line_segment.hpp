#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "cross.hpp"

inline bool intersect_line_segment(const Line& line, const Segment& segment){
    const Point direction = line.b - line.a;
    const long double length = abs(direction);
    if(geometry_sign(length) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    const Point unit_direction = direction / length;
    return geometry_sign(cross(unit_direction, segment.a - line.a)) *
        geometry_sign(cross(unit_direction, segment.b - line.a)) <= 0;
}
