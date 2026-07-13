#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

#include "norm.hpp"
#include "projection.hpp"
#include "unit.hpp"
#include "validate_circle.hpp"

inline std::vector<Point> circle_line_cross_points(
    const Circle& circle,
    const Line& line
){
    validate_circle(circle);
    const Point foot = projection(line, circle.center);
    const long double height_squared =
        circle.radius * circle.radius - norm(foot - circle.center);
    if(geometry_sign(height_squared) < 0) return {};
    if(geometry_sign(height_squared) == 0) return {foot};
    const Point direction = unit(line.b - line.a);
    const long double height =
        std::sqrt(std::max<long double>(0.0L, height_squared));
    std::vector<Point> result = {
        foot - direction * height,
        foot + direction * height,
    };
    std::sort(result.begin(), result.end());
    return result;
}
