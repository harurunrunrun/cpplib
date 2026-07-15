#pragma once

#include <variant>

#include "base.hpp"
#include "line_triangle_intersection.hpp"
#include "linear_intersection3.hpp"
#include "on_ray.hpp"
#include "ray3_direction.hpp"
#include "ray_segment_intersection.hpp"

inline LinearIntersection3 ray_triangle_intersection(
    const Ray3& ray,
    const Triangle3& triangle
){
    (void)ray3_direction(ray);
    const LinearIntersection3 support = line_triangle_intersection(
        Line3{ray.origin, ray.through}, triangle
    );
    if(const auto* point = std::get_if<Point3>(&support)){
        return on_ray(ray, *point) ? LinearIntersection3{*point}
            : LinearIntersection3{std::monostate{}};
    }
    if(const auto* segment = std::get_if<Segment3>(&support)){
        return ray_segment_intersection(ray, *segment);
    }
    return std::monostate{};
}
