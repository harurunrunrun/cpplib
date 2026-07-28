#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_RAY_SEGMENT_RAY_SEGMENT_SKEW_RAY_SEGMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_RAY_SEGMENT_RAY_SEGMENT_SKEW_RAY_SEGMENT_HPP_INCLUDED

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../line_line/line_line_skew.hpp"
#include "../../point/segment/segment3_direction.hpp"

inline bool skew(const Ray3& first, const Segment3& second){
    (void)segment3_direction(second);
    return skew(
        Line3{first.origin, first.through},
        Line3{second.a, second.b}
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_RAY_SEGMENT_RAY_SEGMENT_SKEW_RAY_SEGMENT_HPP_INCLUDED
