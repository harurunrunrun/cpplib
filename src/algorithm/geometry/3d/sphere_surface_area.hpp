#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"

inline long double sphere_surface_area(const Sphere3& sphere){
    if(sphere.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    return 4 * GEOMETRY3D_PI * sphere.radius * sphere.radius;
}
