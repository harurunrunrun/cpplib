#pragma once

#include "circle_segment_cross_points.hpp"

inline int circle_segment_intersection_count(
    const Circle& circle,
    const Segment& segment
){
    return static_cast<int>(
        circle_segment_cross_points(circle, segment).size()
    );
}
