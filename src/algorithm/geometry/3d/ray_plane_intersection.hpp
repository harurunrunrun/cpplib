#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "plane3_unit_normal.hpp"
#include "ray3_direction.hpp"

inline std::optional<Point3> ray_plane_intersection(
    const Ray3& ray,
    const Plane3& plane
){
    const Point3 direction = ray3_direction(ray);
    const Point3 normal = plane3_unit_normal(plane);
    const long double denominator = dot(direction, normal);
    if(geometry3d_sign(denominator) == 0) return std::nullopt;
    const long double parameter =
        dot(plane.point - ray.origin, normal) / denominator;
    if(geometry3d_sign(parameter) < 0) return std::nullopt;
    return ray.origin + direction * parameter;
}
