#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "../2d/types.hpp"
#include "abs.hpp"
#include "base.hpp"
#include "geometry3d_sign.hpp"

enum class DominantAxis3{ x, y, z };

inline DominantAxis3 dominant_axis(const Point3& normal){
    if(geometry3d_sign(abs(normal)) == 0)[[unlikely]]{
        throw std::invalid_argument("zero normal");
    }
    const long double x = std::abs(normal.x);
    const long double y = std::abs(normal.y);
    const long double z = std::abs(normal.z);
    if(x >= y && x >= z) return DominantAxis3::x;
    if(y >= z) return DominantAxis3::y;
    return DominantAxis3::z;
}

inline Point dominant_axis_projection(
    const Point3& point,
    DominantAxis3 axis
){
    if(axis == DominantAxis3::x) return {point.y, point.z};
    if(axis == DominantAxis3::y) return {point.z, point.x};
    return {point.x, point.y};
}
