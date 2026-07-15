#pragma once

#include <variant>

#include "base.hpp"
#include "is_zero_vector.hpp"
#include "line3_direction.hpp"
#include "line_line_intersection.hpp"
#include "linear_intersection3.hpp"
#include "on_line.hpp"
#include "on_segment.hpp"

inline LinearIntersection3 line_segment_intersection(
    const Line3& line,
    const Segment3& segment
){
    (void)line3_direction(line);
    if(is_zero_vector(segment.b - segment.a)){
        return on_line(line, segment.a) ? LinearIntersection3{segment.a}
            : LinearIntersection3{std::monostate{}};
    }
    const LinearIntersection3 support = line_line_intersection(
        line, Line3{segment.a, segment.b}
    );
    if(const auto* point = std::get_if<Point3>(&support)){
        return on_segment(segment, *point) ? LinearIntersection3{*point}
            : LinearIntersection3{std::monostate{}};
    }
    if(std::holds_alternative<Line3>(support)) return segment;
    return std::monostate{};
}
