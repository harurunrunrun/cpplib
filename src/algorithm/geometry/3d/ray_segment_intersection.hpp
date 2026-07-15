#pragma once

#include <algorithm>
#include <variant>

#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "is_zero_vector.hpp"
#include "line_line_intersection.hpp"
#include "linear_intersection3.hpp"
#include "norm.hpp"
#include "on_ray.hpp"
#include "on_segment.hpp"
#include "ray3_direction.hpp"

inline LinearIntersection3 ray_segment_intersection(
    const Ray3& ray,
    const Segment3& segment
){
    const Point3 direction = ray3_direction(ray);
    if(is_zero_vector(segment.b - segment.a)){
        return on_ray(ray, segment.a) ? LinearIntersection3{segment.a}
            : LinearIntersection3{std::monostate{}};
    }
    const LinearIntersection3 support = line_line_intersection(
        Line3{ray.origin, ray.through}, Line3{segment.a, segment.b}
    );
    if(const auto* point = std::get_if<Point3>(&support)){
        return on_ray(ray, *point) && on_segment(segment, *point)
            ? LinearIntersection3{*point}
            : LinearIntersection3{std::monostate{}};
    }
    if(!std::holds_alternative<Line3>(support)) return std::monostate{};

    const long double denominator = norm(direction);
    const long double first_parameter = dot(segment.a - ray.origin, direction)
        / denominator;
    const long double second_parameter = dot(segment.b - ray.origin, direction)
        / denominator;
    const long double lower = std::min(first_parameter, second_parameter);
    const long double upper = std::max(first_parameter, second_parameter);
    if(geometry3d_sign(upper) < 0) return std::monostate{};
    const long double clipped_lower = std::max(0.0L, lower);
    if(geometry3d_sign(upper - clipped_lower) == 0){
        return ray.origin + direction * clipped_lower;
    }
    return Segment3{
        ray.origin + direction * clipped_lower,
        ray.origin + direction * upper,
    };
}
