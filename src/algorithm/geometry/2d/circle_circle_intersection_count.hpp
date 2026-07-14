#pragma once

#include "circle_relation.hpp"

inline constexpr int CIRCLE_INTERSECTION_INFINITE = -1;

inline int circle_circle_intersection_count(
    const Circle& first,
    const Circle& second
){
    const int relation = circle_relation(first, second);
    if(relation == CIRCLE_COINCIDENT){
        return first.radius == 0.0L && second.radius == 0.0L
            ? 1
            : CIRCLE_INTERSECTION_INFINITE;
    }
    if(relation == CIRCLE_INSIDE || relation == CIRCLE_SEPARATE) return 0;
    if(relation == CIRCLE_INTERNALLY_TANGENT
        || relation == CIRCLE_EXTERNALLY_TANGENT){
        return 1;
    }
    return 2;
}
