#pragma once

#include <algorithm>
#include <variant>

#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "line_line_intersection.hpp"
#include "linear_intersection3.hpp"
#include "norm.hpp"
#include "on_ray.hpp"
#include "ray3_direction.hpp"

inline LinearIntersection3 ray_ray_intersection(
    const Ray3& first,
    const Ray3& second
){
    const Point3 first_direction = ray3_direction(first);
    const Point3 second_direction = ray3_direction(second);
    const LinearIntersection3 support = line_line_intersection(
        Line3{first.origin, first.through},
        Line3{second.origin, second.through}
    );
    if(const auto* point = std::get_if<Point3>(&support)){
        return on_ray(first, *point) && on_ray(second, *point)
            ? LinearIntersection3{*point}
            : LinearIntersection3{std::monostate{}};
    }
    if(!std::holds_alternative<Line3>(support)) return std::monostate{};

    const long double second_origin_parameter = dot(
        second.origin - first.origin, first_direction
    ) / norm(first_direction);
    if(geometry3d_sign(dot(first_direction, second_direction)) > 0){
        if(geometry3d_sign(second_origin_parameter) > 0) return second;
        return first;
    }
    const int endpoint_sign = geometry3d_sign(second_origin_parameter);
    if(endpoint_sign < 0) return std::monostate{};
    if(endpoint_sign == 0) return first.origin;
    return Segment3{first.origin, second.origin};
}
