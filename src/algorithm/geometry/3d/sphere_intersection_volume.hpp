#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "abs.hpp"
#include "geometry3d_sign.hpp"

inline long double sphere_intersection_volume(
    const Sphere3& first,
    const Sphere3& second
){
    if(first.radius < 0 || second.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    const long double center_distance = abs(second.center - first.center);
    if(geometry3d_sign(center_distance - first.radius - second.radius) >= 0){
        return 0;
    }
    if(geometry3d_sign(center_distance - std::abs(first.radius - second.radius)) <= 0){
        const long double radius = std::min(first.radius, second.radius);
        return 4 * GEOMETRY3D_PI * radius * radius * radius / 3;
    }
    const long double sum = first.radius + second.radius;
    const long double difference = first.radius - second.radius;
    const long double overlap = sum - center_distance;
    return GEOMETRY3D_PI * overlap * overlap * (
        center_distance * center_distance + 2 * center_distance * sum -
        3 * difference * difference
    ) / (12 * center_distance);
}
