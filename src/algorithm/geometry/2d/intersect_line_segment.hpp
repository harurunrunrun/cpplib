#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "cross.hpp"

inline bool intersect_line_segment(const Line& line, const Segment& segment){
    if(geometry_sign(abs(line.b - line.a)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    return geometry_sign(cross(line.b - line.a, segment.a - line.a)) *
        geometry_sign(cross(line.b - line.a, segment.b - line.a)) <= 0;
}
