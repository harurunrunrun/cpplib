#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "cross.hpp"

inline int side_of_directed_line(const Line& line, const Point& point){
    const Point direction = line.b - line.a;
    if(geometry_sign(abs(direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    return geometry_sign(cross(direction, point - line.a));
}
