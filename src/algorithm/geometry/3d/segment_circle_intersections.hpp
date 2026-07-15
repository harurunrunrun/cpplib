#pragma once

#include <stdexcept>
#include <vector>

#include "base.hpp"
#include "line_circle_intersections.hpp"
#include "on_circle.hpp"
#include "on_segment.hpp"
#include "plane3_unit_normal.hpp"

inline std::vector<Point3> segment_circle_intersections(
    const Segment3& segment,
    const Circle3& circle
){
    if(circle.radius < 0)[[unlikely]]{
        throw std::invalid_argument("negative circle radius");
    }
    (void)plane3_unit_normal({circle.center, circle.normal});
    if(segment.a == segment.b){
        return on_circle(circle, segment.a)
            ? std::vector<Point3>{segment.a} : std::vector<Point3>{};
    }
    std::vector<Point3> result;
    for(const Point3& point: line_circle_intersections(
        Line3{segment.a, segment.b}, circle
    )){
        if(on_segment(segment, point)) result.push_back(point);
    }
    return result;
}
