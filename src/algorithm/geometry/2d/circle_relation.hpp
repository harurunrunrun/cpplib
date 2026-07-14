#pragma once

#include <cmath>

#include "distance.hpp"
#include "validate_circle.hpp"

enum CircleRelation{
    CIRCLE_INSIDE = 0,
    CIRCLE_INTERNALLY_TANGENT = 1,
    CIRCLE_INTERSECT = 2,
    CIRCLE_EXTERNALLY_TANGENT = 3,
    CIRCLE_SEPARATE = 4,
    CIRCLE_COINCIDENT = 5,
};

inline int circle_relation(const Circle& first, const Circle& second){
    validate_circle(first);
    validate_circle(second);
    const long double center_distance = distance(first.center, second.center);
    const long double radius_sum = first.radius + second.radius;
    const long double radius_difference = std::abs(first.radius - second.radius);
    const long double scale = center_distance + radius_sum + radius_difference;
    const int center_sign = circle_numeric_detail::scaled_sign(
        center_distance, scale
    );
    const int radius_difference_sign = circle_numeric_detail::scaled_sign(
        radius_difference, scale
    );
    if(center_sign == 0 && radius_difference_sign == 0){
        return CIRCLE_COINCIDENT;
    }
    const int sum_comparison = circle_numeric_detail::compare(
        center_distance, radius_sum
    );
    if(sum_comparison > 0){
        return CIRCLE_SEPARATE;
    }
    if(sum_comparison == 0){
        return CIRCLE_EXTERNALLY_TANGENT;
    }
    const int difference_comparison = circle_numeric_detail::compare(
        center_distance, radius_difference
    );
    if(difference_comparison == 0){
        return CIRCLE_INTERNALLY_TANGENT;
    }
    if(difference_comparison < 0){
        return CIRCLE_INSIDE;
    }
    return CIRCLE_INTERSECT;
}
