#pragma once

#include <algorithm>

#include "distance.hpp"
#include "validate_circle.hpp"

inline bool intersect_circle_segment(
    const Circle& circle,
    const Segment& segment
){
    validate_circle(circle);
    const long double minimum_distance =
        distance_segment_point(segment, circle.center);
    const long double maximum_distance = std::max(
        distance(circle.center, segment.a),
        distance(circle.center, segment.b)
    );
    return geometry_sign(minimum_distance - circle.radius) <= 0 &&
        geometry_sign(circle.radius - maximum_distance) <= 0;
}
