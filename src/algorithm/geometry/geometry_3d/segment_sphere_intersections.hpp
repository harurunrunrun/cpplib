#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"
#include "abs.hpp"
#include "geometry3d_sign.hpp"
#include "line_sphere_intersections.hpp"
#include "on_segment.hpp"

inline std::vector<Point3> segment_sphere_intersections(
    const Segment3& segment,
    const Sphere3& sphere
){
    if(segment.a == segment.b){
        if(geometry3d_sign(abs(segment.a - sphere.center) - sphere.radius) == 0){
            return {segment.a};
        }
        return {};
    }
    std::vector<Point3> result;
    for(const Point3& point: line_sphere_intersections(
        {segment.a, segment.b}, sphere
    )){
        if(on_segment(segment, point)) result.push_back(point);
    }
    return result;
}
