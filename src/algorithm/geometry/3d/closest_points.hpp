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
#include "line3_direction.hpp"
#include "norm.hpp"
#include "projection.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Line3& first,
    const Line3& second
){
    const Point3 first_direction = line3_direction(first);
    const Point3 second_direction = line3_direction(second);
    const Point3 offset = first.a - second.a;
    const long double aa = norm(first_direction);
    const long double bb = dot(first_direction, second_direction);
    const long double cc = norm(second_direction);
    const long double dd = dot(first_direction, offset);
    const long double ee = dot(second_direction, offset);
    const long double denominator = aa * cc - bb * bb;
    if(geometry3d_sign(denominator) == 0){
        const Point3 on_second = projection(second, first.a);
        return {first.a, on_second};
    }
    const long double first_parameter = (bb * ee - cc * dd) / denominator;
    const long double second_parameter = (aa * ee - bb * dd) / denominator;
    return {
        first.a + first_direction * first_parameter,
        second.a + second_direction * second_parameter,
    };
}

inline std::pair<Point3, Point3> closest_points(
    const Segment3& first,
    const Segment3& second
){
    const Point3 first_direction = first.b - first.a;
    const Point3 second_direction = second.b - second.a;
    const Point3 offset = first.a - second.a;
    const long double aa = norm(first_direction);
    const long double ee = norm(second_direction);
    const long double ff = dot(second_direction, offset);

    long double first_parameter = 0;
    long double second_parameter = 0;
    if(geometry3d_sign(aa) == 0 && geometry3d_sign(ee) == 0){
        return {first.a, second.a};
    }
    if(geometry3d_sign(aa) == 0){
        second_parameter = std::clamp(ff / ee, 0.0L, 1.0L);
    }else{
        const long double cc = dot(first_direction, offset);
        if(geometry3d_sign(ee) == 0){
            first_parameter = std::clamp(-cc / aa, 0.0L, 1.0L);
        }else{
            const long double bb = dot(first_direction, second_direction);
            const long double denominator = aa * ee - bb * bb;
            if(geometry3d_sign(denominator) != 0){
                first_parameter = std::clamp(
                    (bb * ff - cc * ee) / denominator, 0.0L, 1.0L
                );
            }
            second_parameter = (bb * first_parameter + ff) / ee;
            if(second_parameter < 0){
                second_parameter = 0;
                first_parameter = std::clamp(-cc / aa, 0.0L, 1.0L);
            }else if(second_parameter > 1){
                second_parameter = 1;
                first_parameter = std::clamp((bb - cc) / aa, 0.0L, 1.0L);
            }
        }
    }
    return {
        first.a + first_direction * first_parameter,
        second.a + second_direction * second_parameter,
    };
}
