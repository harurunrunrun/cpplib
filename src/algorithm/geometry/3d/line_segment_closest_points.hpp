#pragma once

#include <utility>

#include "base.hpp"
#include "closest_points.hpp"
#include "is_zero_vector.hpp"
#include "norm.hpp"
#include "on_segment.hpp"
#include "projection.hpp"

inline std::pair<Point3, Point3> closest_points(
    const Line3& line,
    const Segment3& segment
){
    if(is_zero_vector(segment.b - segment.a)){
        return {projection(line, segment.a), segment.a};
    }
    const auto support = closest_points(
        line, Line3{segment.a, segment.b}
    );
    if(on_segment(segment, support.second)) return support;
    const Point3 first_projection = projection(line, segment.a);
    const Point3 second_projection = projection(line, segment.b);
    if(norm(first_projection - segment.a) <= norm(second_projection - segment.b)){
        return {first_projection, segment.a};
    }
    return {second_projection, segment.b};
}

inline std::pair<Point3, Point3> closest_points(
    const Segment3& segment,
    const Line3& line
){
    const auto result = closest_points(line, segment);
    return {result.second, result.first};
}
