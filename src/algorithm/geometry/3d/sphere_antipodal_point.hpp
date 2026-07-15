#pragma once

#include <stdexcept>

#include "base.hpp"
#include "on_sphere.hpp"

inline Point3 sphere_antipodal_point(
    const Sphere3& sphere,
    const Point3& point
){
    if(!on_sphere(sphere, point))[[unlikely]]{
        throw std::invalid_argument("point is not on sphere");
    }
    return sphere.center * 2 - point;
}
