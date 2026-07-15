#pragma once

#include "base.hpp"
#include "sphere_closest_point.hpp"

inline Point3 projection(const Sphere3& sphere, const Point3& point){
    return closest_point(sphere, point);
}
