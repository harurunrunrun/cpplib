#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_SKEW_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_SKEW_HPP_INCLUDED

#include "geometry_primitives.hpp"
#include "line_line_skew.hpp"

inline bool skew(const Ray3& first, const Ray3& second){
    return skew(
        Line3{first.origin, first.through},
        Line3{second.origin, second.through}
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_SKEW_HPP_INCLUDED
