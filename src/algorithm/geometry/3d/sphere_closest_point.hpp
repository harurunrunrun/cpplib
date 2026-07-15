#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "base.hpp"
#include "geometry3d_sign.hpp"

inline Point3 closest_point(const Sphere3& sphere, const Point3& point){
    if(sphere.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    const Point3 offset = point - sphere.center;
    const long double center_distance = abs(offset);
    if(geometry3d_sign(sphere.radius) == 0) return sphere.center;
    if(geometry3d_sign(center_distance) == 0)[[unlikely]]{
        throw std::domain_error("closest point on sphere is not unique");
    }
    return sphere.center + offset * (sphere.radius / center_distance);
}
