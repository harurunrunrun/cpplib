#pragma once

#include "cross.hpp"
#include "dot.hpp"
#include "norm.hpp"

enum CcwResult{
    CLOCKWISE = -1,
    COUNTER_CLOCKWISE = 1,
    ONLINE_BACK = 2,
    ONLINE_FRONT = -2,
    ON_SEGMENT = 0,
};

inline int ccw(Point first, Point second, Point third){
    second -= first;
    third -= first;
    const int product_sign = geometry_sign(cross(second, third));
    if(product_sign > 0) return COUNTER_CLOCKWISE;
    if(product_sign < 0) return CLOCKWISE;
    if(geometry_sign(dot(second, third)) < 0) return ONLINE_BACK;
    if(geometry_sign(norm(second) - norm(third)) < 0) return ONLINE_FRONT;
    return ON_SEGMENT;
}
