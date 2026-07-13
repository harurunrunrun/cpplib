#pragma once

#include "circle_relation.hpp"

inline bool intersect_circle_circle(const Circle& first, const Circle& second){
    const int relation = circle_relation(first, second);
    return relation != CIRCLE_INSIDE && relation != CIRCLE_SEPARATE;
}
