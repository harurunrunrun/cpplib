#pragma once

#include <cmath>
#include <stdexcept>

#include "constants.hpp"
#include "types.hpp"

namespace circle_numeric_detail{

inline int scaled_sign(long double value, long double scale){
    if(!std::isfinite(value) || !std::isfinite(scale))[[unlikely]]{
        throw std::invalid_argument(
            "non-finite intermediate in circle geometry comparison"
        );
    }
    return geometry_scaled_sign(value, scale, scale);
}

inline int compare(long double first, long double second){
    return scaled_sign(first - second, std::fabs(first) + std::fabs(second));
}

}  // namespace circle_numeric_detail

inline void validate_circle(const Circle& circle){
    if(!std::isfinite(circle.center.x)
        || !std::isfinite(circle.center.y)
        || !std::isfinite(circle.radius))[[unlikely]]{
        throw std::invalid_argument("circle components must be finite");
    }
    if(circle.radius < 0.0L){
        throw std::invalid_argument("negative circle radius");
    }
}
