#pragma once

#include <vector>

#include "ccw.hpp"
#include "circle_line_cross_points.hpp"
#include "distance.hpp"
#include "validate_circle.hpp"

inline std::vector<Point> circle_segment_cross_points(
    const Circle& circle,
    const Segment& segment
){
    validate_circle(circle);
    if(geometry_sign(abs(segment.b - segment.a)) == 0){
        return geometry_sign(distance(circle.center, segment.a) -
                             circle.radius) == 0
            ? std::vector<Point>{segment.a}
            : std::vector<Point>{};
    }
    std::vector<Point> result;
    for(const Point& point: circle_line_cross_points(circle, segment)){
        if(ccw(segment.a, segment.b, point) == ON_SEGMENT){
            result.push_back(point);
        }
    }
    return result;
}
