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

inline std::optional<Circle3> sphere_sphere_intersection(
    const Sphere3& first,
    const Sphere3& second
){
    if(first.radius < 0 || second.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    const Point3 difference = second.center - first.center;
    const long double center_distance = abs(difference);
    if(geometry3d_sign(center_distance) == 0){
        if(geometry3d_sign(first.radius - second.radius) == 0)[[unlikely]]{
            throw std::domain_error("coincident spheres have no unique intersection circle");
        }
        return std::nullopt;
    }
    if(
        geometry3d_sign(center_distance - first.radius - second.radius) > 0 ||
        geometry3d_sign(center_distance - std::abs(first.radius - second.radius)) < 0
    ){
        return std::nullopt;
    }
    const Point3 normal = difference / center_distance;
    const long double offset = (
        first.radius * first.radius - second.radius * second.radius +
        center_distance * center_distance
    ) / (2 * center_distance);
    const Point3 center = first.center + normal * offset;
    const long double squared_radius = first.radius * first.radius - offset * offset;
    return Circle3{
        center,
        normal,
        std::sqrt(std::max(0.0L, squared_radius)),
    };
}
