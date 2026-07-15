#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "base.hpp"

inline long double sphere_sphere_distance(
    const Sphere3& first,
    const Sphere3& second
){
    const auto finite_sphere = [](const Sphere3& sphere){
        return std::isfinite(sphere.center.x) &&
            std::isfinite(sphere.center.y) &&
            std::isfinite(sphere.center.z) &&
            std::isfinite(sphere.radius);
    };
    if(!finite_sphere(first) || !finite_sphere(second))[[unlikely]]{
        throw std::invalid_argument(
            "sphere_sphere_distance requires finite spheres"
        );
    }
    if(first.radius < 0.0L || second.radius < 0.0L)[[unlikely]]{
        throw std::invalid_argument(
            "sphere_sphere_distance requires nonnegative radii"
        );
    }

    const long double scale = std::max({
        std::abs(first.center.x),
        std::abs(first.center.y),
        std::abs(first.center.z),
        std::abs(second.center.x),
        std::abs(second.center.y),
        std::abs(second.center.z),
        first.radius,
        second.radius,
    });
    if(scale == 0.0L) return 0.0L;

    const auto normalized_difference = [scale](long double left, long double right){
        // Subtraction cannot overflow for equal signs.  Dividing first is needed
        // for opposite signs, where the mathematical difference may exceed the
        // range of long double.
        if(std::signbit(left) == std::signbit(right)){
            return (left - right) / scale;
        }
        return left / scale - right / scale;
    };
    const long double dx = normalized_difference(
        first.center.x,
        second.center.x
    );
    const long double dy = normalized_difference(
        first.center.y,
        second.center.y
    );
    const long double dz = normalized_difference(
        first.center.z,
        second.center.z
    );
    const long double center_distance = std::hypot(dx, dy, dz);
    const long double radius_sum =
        first.radius / scale + second.radius / scale;
    const long double radius_difference =
        std::abs(first.radius - second.radius) / scale;
    const long double normalized_result = std::max({
        0.0L,
        center_distance - radius_sum,
        radius_difference - center_distance,
    });

    if(normalized_result > std::numeric_limits<long double>::max() / scale)
        [[unlikely]]{
        throw std::overflow_error(
            "sphere_sphere_distance result is not representable"
        );
    }
    return normalized_result * scale;
}

inline long double distance(const Sphere3& first, const Sphere3& second){
    return sphere_sphere_distance(first, second);
}
