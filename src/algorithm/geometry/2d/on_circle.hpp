#pragma once

#include "distance.hpp"
#include "validate_circle.hpp"

inline bool on_circle(const Circle& circle, const Point& point){
    validate_circle(circle);
    return geometry_sign(distance(circle.center, point) - circle.radius) == 0;
}
