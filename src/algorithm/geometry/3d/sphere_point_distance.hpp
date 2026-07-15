#pragma once

#include <cmath>
#include <stdexcept>

#include "abs.hpp"
#include "base.hpp"

inline long double distance(const Sphere3& sphere, const Point3& point){
    if(sphere.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    return std::abs(abs(point - sphere.center) - sphere.radius);
}
