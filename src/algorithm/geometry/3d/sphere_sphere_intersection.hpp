#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "is_finite.hpp"
#include "abs.hpp"
#include "geometry3d_sign.hpp"

inline std::optional<Circle3> sphere_sphere_intersection(
    const Sphere3& first,
    const Sphere3& second
){
    geometry3d_validate(first);
    geometry3d_validate(second);
    const auto center_difference = geometry3d_normalized_difference(
        second.center, first.center, {first.radius, second.radius}
    );
    const long double scale = center_difference.scale;
    const long double first_radius = first.radius / scale;
    const long double second_radius = second.radius / scale;
    const Point3 difference = center_difference.value;
    const long double center_distance = std::hypot(
        difference.x, difference.y, difference.z
    );
    const long double linear_scale = std::max({
        center_distance, first_radius, second_radius,
    });
    if(geometry3d_scaled_sign(center_distance, linear_scale) == 0){
        if(geometry3d_scaled_sign(
            first_radius - second_radius, linear_scale
        ) == 0)[[unlikely]]{
            throw std::domain_error("coincident spheres have no unique intersection circle");
        }
        return std::nullopt;
    }
    if(geometry3d_scaled_sign(
        center_distance - first_radius - second_radius, linear_scale
    ) > 0 || geometry3d_scaled_sign(
        center_distance - std::abs(first_radius - second_radius), linear_scale
    ) < 0){
        return std::nullopt;
    }
    const Point3 normal = difference / center_distance;
    const long double offset = (
        (first_radius - second_radius) * (first_radius + second_radius) +
        center_distance * center_distance
    ) / (2 * center_distance);
    const Point3 normalized_center = normal * offset;
    const long double squared_radius =
        (first_radius - offset) * (first_radius + offset);
    if(geometry3d_scaled_sign(
        squared_radius, std::max(first_radius * first_radius, offset * offset)
    ) < 0) return std::nullopt;
    const Point3 center{
        std::fma(normalized_center.x, scale, first.center.x),
        std::fma(normalized_center.y, scale, first.center.y),
        std::fma(normalized_center.z, scale, first.center.z),
    };
    const long double radius = std::sqrt(
        std::max(0.0L, squared_radius)
    ) * scale;
    if(!geometry3d_is_finite(center) || !std::isfinite(radius))[[unlikely]]{
        throw std::overflow_error(
            "sphere-sphere intersection circle is not representable"
        );
    }
    return Circle3{center, normal, radius};
}
