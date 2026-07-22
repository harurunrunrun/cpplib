#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_TRIANGLE_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_TRIANGLE_INTERSECTION_HPP_INCLUDED

#include <variant>

#include "base.hpp"
#include "contains.hpp"
#include "is_zero_vector.hpp"
#include "line_triangle_intersection.hpp"
#include "linear_intersection3.hpp"
#include "on_segment.hpp"
#include "segment_segment_intersection.hpp"

inline LinearIntersection3 segment_triangle_intersection(
    const Segment3& segment,
    const Triangle3& triangle
){
    if(is_zero_vector(segment.b - segment.a)){
        return contains(triangle, segment.a)
            ? LinearIntersection3{segment.a}
            : LinearIntersection3{std::monostate{}};
    }
    const LinearIntersection3 support = line_triangle_intersection(
        Line3{segment.a, segment.b}, triangle
    );
    if(const auto* point = std::get_if<Point3>(&support)){
        return on_segment(segment, *point) ? LinearIntersection3{*point}
            : LinearIntersection3{std::monostate{}};
    }
    if(const auto* triangle_segment = std::get_if<Segment3>(&support)){
        return segment_segment_intersection(segment, *triangle_segment);
    }
    return std::monostate{};
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SEGMENT_TRIANGLE_INTERSECTION_HPP_INCLUDED
