#pragma once

#include "cross.hpp"
#include "dot.hpp"

inline bool on_segment(const Segment& segment, const Point& point){
    return geometry_sign(cross(segment.b - segment.a, point - segment.a)) == 0 &&
        geometry_sign(dot(point - segment.a, point - segment.b)) <= 0;
}
