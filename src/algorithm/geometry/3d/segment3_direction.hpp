#pragma once

#include <stdexcept>

#include "base.hpp"
#include "is_zero_vector.hpp"

inline Point3 segment3_direction(const Segment3& segment){
    const Point3 direction = segment.b - segment.a;
    if(is_zero_vector(direction))[[unlikely]]{
        throw std::invalid_argument("degenerate 3D segment has no direction");
    }
    return direction;
}
