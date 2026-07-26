#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_SKEW_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_SKEW_HPP_INCLUDED

#include "geometry_primitives.hpp"
#include "line_line_skew.hpp"

inline bool skew(const Line3& first, const Ray3& second){
    return skew(first, Line3{second.origin, second.through});
}

inline bool skew(const Ray3& first, const Line3& second){
    return skew(second, first);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_SKEW_HPP_INCLUDED
