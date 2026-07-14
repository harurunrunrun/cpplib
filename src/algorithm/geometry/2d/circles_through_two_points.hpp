#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "distance.hpp"
#include "rotate90.hpp"
#include "unit.hpp"
#include "validate_circle.hpp"

inline std::vector<Circle> circles_through_two_points(
    const Point& first,
    const Point& second,
    long double radius
){
    validate_circle({{}, radius});
    const long double chord_length = distance(first, second);
    if(geometry_sign(chord_length) == 0){
        if(geometry_sign(radius) == 0) return {{first, radius}};
        throw std::domain_error(
            "infinitely many circles pass through one point"
        );
    }
    if(geometry_sign(chord_length - 2.0L * radius) > 0) return {};

    const Point midpoint = (first + second) / 2.0L;
    long double height_squared = radius * radius
        - chord_length * chord_length / 4.0L;
    if(geometry_sign(height_squared) == 0) return {{midpoint, radius}};
    height_squared = std::max(0.0L, height_squared);
    const Point offset = rotate90(unit(second - first))
        * std::sqrt(height_squared);
    std::vector<Circle> result = {
        {midpoint - offset, radius},
        {midpoint + offset, radius},
    };
    std::sort(result.begin(), result.end(), [](const Circle& left, const Circle& right){
        return left.center < right.center;
    });
    return result;
}
