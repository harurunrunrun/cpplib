#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_SEGMENT_TETRAHEDRON_SEGMENT_TETRAHEDRON_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_SEGMENT_TETRAHEDRON_SEGMENT_TETRAHEDRON_INTERSECTION_HPP_INCLUDED

#include <variant>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../aggregate/all/contains.hpp"
#include "../../predicate/point/is_zero_vector.hpp"
#include "../line_tetrahedron/line_tetrahedron_intersection.hpp"
#include "../../type/definition/linear_intersection3.hpp"
#include "../../predicate/segment_point/on_segment.hpp"
#include "../segment_segment/segment_segment_intersection.hpp"

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

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_SEGMENT_TETRAHEDRON_SEGMENT_TETRAHEDRON_INTERSECTION_HPP_INCLUDED
