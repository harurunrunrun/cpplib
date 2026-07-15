#pragma once

#include <algorithm>
#include <variant>

#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "is_zero_vector.hpp"
#include "line_line_intersection.hpp"
#include "linear_intersection3.hpp"
#include "norm.hpp"
#include "on_segment.hpp"

inline LinearIntersection3 segment_segment_intersection(
    const Segment3& first,
    const Segment3& second
){
    const Point3 first_direction = first.b - first.a;
    const Point3 second_direction = second.b - second.a;
    const bool first_degenerate = is_zero_vector(first_direction);
    const bool second_degenerate = is_zero_vector(second_direction);
    if(first_degenerate && second_degenerate){
        return first.a == second.a ? LinearIntersection3{first.a}
            : LinearIntersection3{std::monostate{}};
    }
    if(first_degenerate){
        return on_segment(second, first.a) ? LinearIntersection3{first.a}
            : LinearIntersection3{std::monostate{}};
    }
    if(second_degenerate){
        return on_segment(first, second.a) ? LinearIntersection3{second.a}
            : LinearIntersection3{std::monostate{}};
    }

    const LinearIntersection3 support = line_line_intersection(
        Line3{first.a, first.b}, Line3{second.a, second.b}
    );
    if(const auto* point = std::get_if<Point3>(&support)){
        return on_segment(first, *point) && on_segment(second, *point)
            ? LinearIntersection3{*point}
            : LinearIntersection3{std::monostate{}};
    }
    if(!std::holds_alternative<Line3>(support)) return std::monostate{};

    const long double denominator = norm(first_direction);
    long double second_first = dot(second.a - first.a, first_direction)
        / denominator;
    long double second_second = dot(second.b - first.a, first_direction)
        / denominator;
    if(second_first > second_second) std::swap(second_first, second_second);
    const long double lower = std::max(0.0L, second_first);
    const long double upper = std::min(1.0L, second_second);
    const int interval_sign = geometry3d_sign(upper - lower);
    if(interval_sign < 0) return std::monostate{};
    if(interval_sign == 0) return first.a + first_direction * lower;
    return Segment3{
        first.a + first_direction * lower,
        first.a + first_direction * upper,
    };
}
