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
#include "is_finite.hpp"

inline bool orthogonal(const Point3& left, const Point3& right){
    if(!geometry3d_is_finite(left) || !geometry3d_is_finite(right))[[unlikely]]{
        throw std::invalid_argument("orthogonal requires finite vectors");
    }
    const long double left_scale = std::max({
        std::abs(left.x), std::abs(left.y), std::abs(left.z)
    });
    const long double right_scale = std::max({
        std::abs(right.x), std::abs(right.y), std::abs(right.z)
    });
    if(left_scale == 0.0L || right_scale == 0.0L) return true;
    const Point3 scaled_left = left / left_scale;
    const Point3 scaled_right = right / right_scale;
    const long double product = std::fma(
        scaled_left.x, scaled_right.x,
        std::fma(
            scaled_left.y, scaled_right.y,
            scaled_left.z * scaled_right.z
        )
    );
    return std::abs(product) <=
        GEOMETRY3D_EPS * abs(scaled_left) * abs(scaled_right);
}
