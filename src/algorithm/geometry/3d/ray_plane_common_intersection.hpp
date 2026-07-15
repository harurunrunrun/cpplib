#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "is_finite.hpp"
#include "linear_intersection3.hpp"
#include "on_plane.hpp"
#include "plane3_unit_normal.hpp"
#include "ray3_direction.hpp"
#include "ray_plane_intersection.hpp"

inline LinearIntersection3 ray_plane_common_intersection(
    const Ray3& ray,
    const Plane3& plane
){
    if(!geometry3d_is_finite(ray) || !geometry3d_is_finite(plane))[[unlikely]]{
        throw std::invalid_argument(
            "ray_plane_common_intersection requires finite inputs"
        );
    }
    Point3 direction = ray3_direction(ray);
    const long double scale = std::max({
        std::abs(direction.x), std::abs(direction.y), std::abs(direction.z)
    });
    direction /= scale;
    const Point3 normal = plane3_unit_normal(plane);
    if(geometry3d_sign(dot(direction, normal)) == 0){
        if(on_plane(plane, ray.origin)) return ray;
        return std::monostate{};
    }
    const auto point = ray_plane_intersection(ray, plane);
    if(point) return *point;
    return std::monostate{};
}
