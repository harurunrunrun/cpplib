#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_SEGMENT_POLYHEDRON_SEGMENT_INTERSECTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_SEGMENT_POLYHEDRON_SEGMENT_INTERSECTS_HPP_INCLUDED

#include <stdexcept>
#include <variant>

#include "../../predicate/polyhedron_point/polyhedron_contains.hpp"
#include "../../predicate/segment/segment3_is_finite.hpp"
#include "../../result/segment_triangle/segment_triangle_intersection.hpp"
#include "../../type/definition/polyhedron3.hpp"

inline bool polyhedron_segment_intersects(
    const Polyhedron3& polyhedron,
    const Segment3& segment
){
    geometry3d_validate(polyhedron);
    if(!geometry3d_is_finite(segment))[[unlikely]]{
        throw std::invalid_argument("non-finite polyhedron query segment");
    }
    if(polyhedron.faces.empty()) return false;
    if(polyhedron_contains_detail::unchecked(polyhedron, segment.a)
        || polyhedron_contains_detail::unchecked(polyhedron, segment.b)){
        return true;
    }
    for(const auto& face: polyhedron.faces){
        const LinearIntersection3 intersection = segment_triangle_intersection(
            segment,
            {polyhedron.vertices[face[0]], polyhedron.vertices[face[1]],
             polyhedron.vertices[face[2]]}
        );
        if(!std::holds_alternative<std::monostate>(intersection)) return true;
    }
    return false;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_SEGMENT_POLYHEDRON_SEGMENT_INTERSECTS_HPP_INCLUDED
