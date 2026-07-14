#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "rotate90.hpp"

inline Line perpendicular_bisector(
    const Point& first,
    const Point& second
){
    const Point direction = second - first;
    if(geometry_sign(abs(direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("two distinct points are required");
    }
    const Point midpoint = (first + second) / 2.0L;
    return {midpoint, midpoint + rotate90(direction)};
}
