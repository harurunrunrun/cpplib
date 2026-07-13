#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "abs.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"

inline long double angle(const Point3& first, const Point3& second){
    const long double denominator = abs(first) * abs(second);
    if(geometry3d_sign(denominator) == 0)[[unlikely]]{
        throw std::invalid_argument("angle with zero 3D vector");
    }
    return std::acos(std::clamp(dot(first, second) / denominator, -1.0L, 1.0L));
}
