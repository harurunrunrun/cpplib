#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_RAY_LINE_RAY_SKEW_LINE_RAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_RAY_LINE_RAY_SKEW_LINE_RAY_HPP_INCLUDED

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../line_line/line_line_skew.hpp"

inline bool skew(const Line3& first, const Ray3& second){
    return skew(first, Line3{second.origin, second.through});
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_RAY_LINE_RAY_SKEW_LINE_RAY_HPP_INCLUDED
