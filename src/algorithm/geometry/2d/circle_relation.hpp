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
    if(geometry_sign(center_distance) == 0 &&
       geometry_sign(radius_difference) == 0){
        return CIRCLE_COINCIDENT;
    }
    if(geometry_sign(center_distance - radius_sum) > 0){
        return CIRCLE_SEPARATE;
    }
    if(geometry_sign(center_distance - radius_sum) == 0){
        return CIRCLE_EXTERNALLY_TANGENT;
    }
    if(geometry_sign(center_distance - radius_difference) == 0){
        return CIRCLE_INTERNALLY_TANGENT;
    }
    if(geometry_sign(center_distance - radius_difference) < 0){
        return CIRCLE_INSIDE;
    }
    return CIRCLE_INTERSECT;
}
