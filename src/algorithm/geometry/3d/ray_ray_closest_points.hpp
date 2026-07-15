#pragma once

#include <algorithm>
#include <array>
#include <limits>
#include <utility>

#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "norm.hpp"
#include "ray3_direction.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Ray3& first,
    const Ray3& second
){
    const Point3 first_direction = ray3_direction(first);
    const Point3 second_direction = ray3_direction(second);
    const Point3 offset = first.origin - second.origin;
    const long double aa = norm(first_direction);
    const long double bb = dot(first_direction, second_direction);
    const long double cc = norm(second_direction);
    const long double dd = dot(first_direction, offset);
    const long double ee = dot(second_direction, offset);
    const long double denominator = aa * cc - bb * bb;

    long double best_distance = std::numeric_limits<long double>::infinity();
    std::pair<Point3, Point3> best;
    const auto consider = [&](long double first_parameter, long double second_parameter){
        if(geometry3d_sign(first_parameter) < 0 ||
            geometry3d_sign(second_parameter) < 0) return;
        first_parameter = std::max(0.0L, first_parameter);
        second_parameter = std::max(0.0L, second_parameter);
        const Point3 on_first = first.origin + first_direction * first_parameter;
        const Point3 on_second = second.origin + second_direction * second_parameter;
        const long double squared_distance = norm(on_first - on_second);
        if(squared_distance < best_distance){
            best_distance = squared_distance;
            best = {on_first, on_second};
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
    consider(0, 0);
    return best;
}
