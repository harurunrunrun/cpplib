#pragma once

#include "base.hpp"
#include "closest_point.hpp"

inline Point3 projection(const Ray3& ray, const Point3& point){
    return closest_point(ray, point);
}
