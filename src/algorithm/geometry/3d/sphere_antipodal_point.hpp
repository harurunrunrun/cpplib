#pragma once

#include <cmath>
#include <stdexcept>

#include "base.hpp"
#include "is_finite.hpp"
#include "on_sphere.hpp"

inline Point3 sphere_antipodal_point(
    const Sphere3& sphere,
    const Point3& point
){
    if(!on_sphere(sphere, point))[[unlikely]]{
        throw std::invalid_argument("point is not on sphere");
    }
    const Geometry3DNormalizedDifference offset =
        geometry3d_normalized_difference(
            point, sphere.center, {sphere.radius}
        );
    const Point3 result{
        std::fma(-offset.value.x, offset.scale, sphere.center.x),
        std::fma(-offset.value.y, offset.scale, sphere.center.y),
        std::fma(-offset.value.z, offset.scale, sphere.center.z),
    };
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error(
            "sphere antipodal point is not representable"
        );
    }
    return result;
}
