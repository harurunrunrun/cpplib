#pragma once

#include <stdexcept>

#include "abs.hpp"
#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "plane3_unit_normal.hpp"

inline Point3 closest_point(const Circle3& circle, const Point3& point){
    if(circle.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative circle radius");
    }
    const Point3 normal = plane3_unit_normal({circle.center, circle.normal});
    const Point3 offset = point - circle.center;
    const Point3 radial = offset - normal * dot(offset, normal);
    const long double radial_length = abs(radial);
    if(geometry3d_sign(circle.radius) == 0) return circle.center;
    if(geometry3d_sign(radial_length) == 0)[[unlikely]]{
        throw std::domain_error("closest point on circle is not unique");
    }
    return circle.center + radial * (circle.radius / radial_length);
}
