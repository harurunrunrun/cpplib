#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "geometry3d_sign.hpp"
#include "norm.hpp"
#include "plane3_unit_normal.hpp"
#include "projection.hpp"

inline std::optional<Circle3> plane_sphere_intersection(
    const Plane3& plane,
    const Sphere3& sphere
){
    if(sphere.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative sphere radius");
    }
    const Point3 center = projection(plane, sphere.center);
    const long double squared_radius =
        sphere.radius * sphere.radius - norm(center - sphere.center);
    if(geometry3d_sign(squared_radius) < 0) return std::nullopt;
    return Circle3{
        center,
        plane3_unit_normal(plane),
        std::sqrt(std::max(0.0L, squared_radius)),
    };
}
