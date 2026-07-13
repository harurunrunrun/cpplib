#pragma once

#include <stdexcept>

#include "types.hpp"

inline void validate_circle(const Circle& circle){
    if(circle.radius < 0.0L){
        throw std::invalid_argument("negative circle radius");
    }
}
