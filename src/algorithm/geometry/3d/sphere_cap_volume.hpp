#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"

inline long double sphere_cap_volume(
    const Sphere3& sphere,
    long double height
){
    if(
        sphere.radius < 0 || height < 0 ||
        height > 2 * sphere.radius
    )[[unlikely]]{
        throw std::invalid_argument("invalid sphere cap height");
    }
    return GEOMETRY3D_PI * height * height * (sphere.radius - height / 3);
}
