#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SEGMENT_SEGMENT_SEGMENT_SEGMENT_COINCIDENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SEGMENT_SEGMENT_SEGMENT_SEGMENT_COINCIDENT_HPP_INCLUDED

#include "../../aggregate/all/geometry_primitives.hpp"

inline bool coincident(const Segment3& first, const Segment3& second){
    return (first.a == second.a && first.b == second.b) ||
        (first.a == second.b && first.b == second.a);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_SEGMENT_SEGMENT_SEGMENT_SEGMENT_COINCIDENT_HPP_INCLUDED
