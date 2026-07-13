#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "abs.hpp"
#include "norm.hpp"
#include "rotate90.hpp"
#include "same_line.hpp"
#include "validate_circle.hpp"

inline std::vector<Line> common_tangents(
    const Circle& first,
    const Circle& second
){
    validate_circle(first);
    validate_circle(second);
    const Point center_difference = second.center - first.center;
    const long double distance_squared = norm(center_difference);
    if(geometry_sign(abs(center_difference)) == 0 &&
       geometry_sign(first.radius - second.radius) == 0){
        throw std::domain_error(
            "coincident circles have infinitely many common tangents"
        );
    }
    if(geometry_sign(abs(center_difference)) == 0) return {};
    std::vector<Line> result;
    for(const int same_side: {-1, 1}){
        const long double radius_difference =
            first.radius - same_side * second.radius;
        const long double height_squared =
            distance_squared - radius_difference * radius_difference;
        if(geometry_sign(height_squared) < 0) continue;
        const long double height =
            std::sqrt(std::max<long double>(0.0L, height_squared));
        for(const int sign: {-1, 1}){
            const Point normal = (
                center_difference * radius_difference +
                rotate90(center_difference) * (height * sign)
            ) / distance_squared;
            const Point first_point =
                first.center + normal * first.radius;
            const Point second_point =
                second.center + normal * (same_side * second.radius);
            const Line tangent = first_point == second_point
                ? Line{first_point, first_point + rotate90(normal)}
                : Line{first_point, second_point};
            const bool duplicate = std::any_of(
                result.begin(), result.end(),
                [&](const Line& line){ return same_line(line, tangent); }
            );
            if(!duplicate) result.push_back(tangent);
            if(geometry_sign(height) == 0) break;
        }
    }
    return result;
}
