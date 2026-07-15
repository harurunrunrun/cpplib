#pragma once

#include <algorithm>
#include <limits>
#include <utility>

#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "is_zero_vector.hpp"
#include "norm.hpp"
#include "ray3_direction.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Ray3& ray,
    const Segment3& segment
){
    const Point3 ray_direction = ray3_direction(ray);
    const Point3 segment_direction = segment.b - segment.a;
    if(is_zero_vector(segment_direction)){
        const long double parameter = std::max(
            0.0L,
            dot(segment.a - ray.origin, ray_direction) / norm(ray_direction)
        );
        return {ray.origin + ray_direction * parameter, segment.a};
    }
    const Point3 offset = ray.origin - segment.a;
    const long double aa = norm(ray_direction);
    const long double bb = dot(ray_direction, segment_direction);
    const long double cc = norm(segment_direction);
    const long double dd = dot(ray_direction, offset);
    const long double ee = dot(segment_direction, offset);
    const long double denominator = aa * cc - bb * bb;

    long double best_distance = std::numeric_limits<long double>::infinity();
    std::pair<Point3, Point3> best;
    const auto consider = [&](long double ray_parameter, long double segment_parameter){
        if(geometry3d_sign(ray_parameter) < 0 ||
            geometry3d_sign(segment_parameter) < 0 ||
            geometry3d_sign(segment_parameter - 1) > 0) return;
        ray_parameter = std::max(0.0L, ray_parameter);
        segment_parameter = std::clamp(segment_parameter, 0.0L, 1.0L);
        const Point3 on_ray = ray.origin + ray_direction * ray_parameter;
        const Point3 on_segment = segment.a + segment_direction * segment_parameter;
        const long double squared_distance = norm(on_ray - on_segment);
        if(squared_distance < best_distance){
            best_distance = squared_distance;
            best = {on_ray, on_segment};
        }
    };
    if(geometry3d_sign(denominator) != 0){
        consider(
            (bb * ee - cc * dd) / denominator,
            (aa * ee - bb * dd) / denominator
        );
    }
    consider(0, ee / cc);
    consider(-dd / aa, 0);
    consider((bb - dd) / aa, 1);
    consider(0, 0);
    consider(0, 1);
    return best;
}

inline std::pair<Point3, Point3> closest_points(
    const Segment3& segment,
    const Ray3& ray
){
    const auto result = closest_points(ray, segment);
    return {result.second, result.first};
}
