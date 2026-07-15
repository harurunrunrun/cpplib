#pragma once

#include <algorithm>
#include <stdexcept>

#include "is_finite.hpp"

inline long double clamp(
    long double value,
    long double lower_bound,
    long double upper_bound
){
    if(!geometry3d_is_finite(value) || !geometry3d_is_finite(lower_bound)
        || !geometry3d_is_finite(upper_bound) || lower_bound > upper_bound)[[unlikely]]{
        throw std::invalid_argument("non-finite value or reversed 3D clamp bounds");
    }
    return std::clamp(value, lower_bound, upper_bound);
}
