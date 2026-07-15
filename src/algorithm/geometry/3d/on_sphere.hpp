#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "base.hpp"
#include "geometry3d_sign.hpp"

inline bool on_sphere(const Sphere3& sphere, const Point3& point){
    if(sphere.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    return geometry3d_sign(abs(point - sphere.center) - sphere.radius) == 0;
}
