#pragma once

#include <algorithm>
#include <stdexcept>

#include "base.hpp"
#include "signed_distance.hpp"

inline long double sphere_cap_height(
    const Sphere3& sphere,
    const Plane3& cutting_plane,
    int side
){
    if(sphere.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    if(side != -1 && side != 1)[[unlikely]]{
        throw std::invalid_argument("sphere cap side must be -1 or 1");
    }
    const long double center_side = signed_distance(cutting_plane, sphere.center);
    return std::clamp(
        sphere.radius + side * center_side,
        0.0L,
        2 * sphere.radius
    );
}
