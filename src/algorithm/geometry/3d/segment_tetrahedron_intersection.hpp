#pragma once

#include <variant>

#include "base.hpp"
#include "contains.hpp"
#include "is_zero_vector.hpp"
#include "line_tetrahedron_intersection.hpp"
#include "linear_intersection3.hpp"
#include "on_segment.hpp"
#include "segment_segment_intersection.hpp"

inline LinearIntersection3 segment_tetrahedron_intersection(
    const Segment3& segment,
    const Tetrahedron3& tetrahedron
){
    if(is_zero_vector(segment.b - segment.a)){
        return contains(tetrahedron, segment.a)
            ? LinearIntersection3{segment.a}
            : LinearIntersection3{std::monostate{}};
    }
    const LinearIntersection3 support = line_tetrahedron_intersection(
        Line3{segment.a, segment.b}, tetrahedron
    );
    if(const auto* point = std::get_if<Point3>(&support)){
        return on_segment(segment, *point) ? LinearIntersection3{*point}
            : LinearIntersection3{std::monostate{}};
    }
    if(const auto* tetrahedron_segment = std::get_if<Segment3>(&support)){
        return segment_segment_intersection(segment, *tetrahedron_segment);
    }
    return std::monostate{};
}
