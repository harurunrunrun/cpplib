#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SPHERE_INTERSECTION_VOLUME_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SPHERE_INTERSECTION_VOLUME_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "../core/geometry_primitives.hpp"
#include "../predicate/is_finite.hpp"
#include "abs.hpp"
#include "../predicate/geometry3d_sign.hpp"

inline long double sphere_intersection_volume(
    const Sphere3& first,
    const Sphere3& second
){
    geometry3d_validate(first);
    geometry3d_validate(second);
    const auto difference = geometry3d_normalized_difference(
        second.center, first.center, {first.radius, second.radius}
    );
    const long double scale = difference.scale;
    const long double center_distance = std::hypot(
        difference.value.x, difference.value.y, difference.value.z
    );
    const long double first_radius = first.radius / scale;
    const long double second_radius = second.radius / scale;
    const long double linear_scale = std::max({
        center_distance, first_radius, second_radius,
    });
    long double normalized_volume = 0.0L;
    if(geometry3d_scaled_sign(
        center_distance - first_radius - second_radius, linear_scale
    ) >= 0){
        return 0.0L;
    }
    if(geometry3d_scaled_sign(
        center_distance - std::abs(first_radius - second_radius), linear_scale
    ) <= 0){
        const long double radius = std::min(first_radius, second_radius);
        normalized_volume = 4 * GEOMETRY3D_PI * radius * radius * radius / 3;
    }else{
        const long double sum = first_radius + second_radius;
        const long double radius_difference = first_radius - second_radius;
        const long double overlap = sum - center_distance;
        normalized_volume = GEOMETRY3D_PI * overlap * overlap * (
            center_distance * center_distance + 2 * center_distance * sum -
            3 * radius_difference * radius_difference
        ) / (12 * center_distance);
    }
    return geometry3d_detail::checked_nonnegative_product(
        {normalized_volume, scale, scale, scale},
        "sphere intersection volume is not representable"
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SCALAR_SPHERE_INTERSECTION_VOLUME_HPP_INCLUDED
