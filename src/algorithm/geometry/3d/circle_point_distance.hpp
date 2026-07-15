#pragma once

#include <cmath>
#include <stdexcept>

#include "abs.hpp"
#include "base.hpp"
#include "dot.hpp"
#include "plane3_unit_normal.hpp"

inline long double distance(const Circle3& circle, const Point3& point){
    if(circle.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative circle radius");
    }
    const Point3 normal = plane3_unit_normal({circle.center, circle.normal});
    const Point3 offset = point - circle.center;
    const long double height = dot(offset, normal);
    const long double radial_distance = abs(offset - normal * height);
    return std::hypot(height, radial_distance - circle.radius);
}
