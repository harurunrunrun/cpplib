#pragma once

#include <cmath>
#include <stdexcept>

#include "base.hpp"
#include "is_finite.hpp"

inline long double sphere_cap_volume(
    const Sphere3& sphere,
    long double height
){
    geometry3d_validate(sphere);
    if(!std::isfinite(height) || height < 0.0L ||
        height / 2.0L > sphere.radius)[[unlikely]]{
        throw std::invalid_argument("invalid sphere cap height");
    }
    const long double factor = sphere.radius - height / 3.0L;
    return geometry3d_detail::checked_nonnegative_product(
        {GEOMETRY3D_PI, height, height, factor},
        "sphere cap volume is not representable"
    );
}
