#pragma once

#include <algorithm>

#include "angle.hpp"
#include "base.hpp"
#include "line3_direction.hpp"

inline long double line_line_angle(const Line3& first, const Line3& second){
    const long double directed = angle(
        line3_direction(first), line3_direction(second)
    );
    return std::min(directed, GEOMETRY3D_PI - directed);
}
