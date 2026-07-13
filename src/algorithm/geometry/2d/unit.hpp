#pragma once

#include <stdexcept>

#include "abs.hpp"

inline Point unit(const Point& point){
    const long double length = abs(point);
    if(geometry_sign(length) == 0)[[unlikely]]{
        throw std::invalid_argument("unit vector of zero vector");
    }
    return point / length;
}
