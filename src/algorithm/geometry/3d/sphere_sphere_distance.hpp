#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "base.hpp"
#include "is_finite.hpp"

inline long double sphere_sphere_distance(
    const Sphere3& first,
    const Sphere3& second
){
    geometry3d_validate(first);
    geometry3d_validate(second);

    const auto difference = geometry3d_normalized_difference(
        first.center, second.center, {first.radius, second.radius}
    );
    const long double center_distance = std::hypot(
        difference.value.x, difference.value.y, difference.value.z
    );
    const long double first_radius = first.radius / difference.scale;
    const long double second_radius = second.radius / difference.scale;
    const long double radius_sum = first_radius + second_radius;
    const long double radius_difference = std::abs(first_radius - second_radius);
    const long double normalized_result = std::max({
        0.0L,
        center_distance - radius_sum,
        radius_difference - center_distance,
    });

    if(normalized_result > std::numeric_limits<long double>::max() / difference.scale)
        [[unlikely]]{
        throw std::overflow_error(
            "sphere_sphere_distance result is not representable"
        );
    }
    return normalized_result * difference.scale;
}

inline long double distance(const Sphere3& first, const Sphere3& second){
    return sphere_sphere_distance(first, second);
}
