#pragma once

#include <utility>

#include "base.hpp"
#include "closest_points.hpp"
#include "on_ray.hpp"
#include "projection.hpp"
#include "ray3_direction.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Line3& line,
    const Ray3& ray
){
    (void)ray3_direction(ray);
    const auto support = closest_points(
        line, Line3{ray.origin, ray.through}
    );
    if(on_ray(ray, support.second)) return support;
    return {projection(line, ray.origin), ray.origin};
}

inline std::pair<Point3, Point3> closest_points(
    const Ray3& ray,
    const Line3& line
){
    const auto result = closest_points(line, ray);
    return {result.second, result.first};
}
