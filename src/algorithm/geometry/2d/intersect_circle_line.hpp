#pragma once

#include "distance.hpp"
#include "validate_circle.hpp"

inline bool intersect_circle_line(const Circle& circle, const Line& line){
    validate_circle(circle);
    return geometry_sign(distance(line, circle.center) - circle.radius) <= 0;
}
