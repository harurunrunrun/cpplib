#pragma once

#include <stdexcept>
#include <algorithm>
#include <cmath>

#include "abs.hpp"
#include "base.hpp"
#include "is_finite.hpp"
#include "geometry3d_sign.hpp"

inline Point3 closest_point(const Sphere3& sphere, const Point3& point){
    geometry3d_validate(sphere);
    if(!geometry3d_is_finite(point))[[unlikely]]{
        throw std::invalid_argument("sphere closest point requires a finite point");
    }
    if(sphere.radius == 0.0L) return sphere.center;
    const auto difference = geometry3d_normalized_difference(
        point, sphere.center, {sphere.radius}
    );
    const long double center_distance = std::hypot(
        difference.value.x, difference.value.y, difference.value.z
    );
    const long double radius = sphere.radius / difference.scale;
    if(geometry3d_scaled_sign(
        center_distance, std::max(center_distance, radius)
    ) == 0)[[unlikely]]{
        throw std::domain_error("closest point on sphere is not unique");
    }
    const long double multiplier = sphere.radius / center_distance;
    const Point3 result{
        std::fma(difference.value.x, multiplier, sphere.center.x),
        std::fma(difference.value.y, multiplier, sphere.center.y),
        std::fma(difference.value.z, multiplier, sphere.center.z),
    };
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error(
            "closest point on sphere is not representable"
        );
    }
    return result;
}
