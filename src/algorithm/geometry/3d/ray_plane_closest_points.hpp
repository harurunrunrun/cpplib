#pragma once

#include <utility>

#include "base.hpp"
#include "projection.hpp"
#include "ray3_direction.hpp"
#include "ray_plane_intersection.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Ray3& ray,
    const Plane3& plane
){
    (void)ray3_direction(ray);
    const auto intersection = ray_plane_intersection(ray, plane);
    if(intersection) return {*intersection, *intersection};
    return {ray.origin, projection(plane, ray.origin)};
}

inline std::pair<Point3, Point3> closest_points(
    const Plane3& plane,
    const Ray3& ray
){
    const auto result = closest_points(ray, plane);
    return {result.second, result.first};
}
