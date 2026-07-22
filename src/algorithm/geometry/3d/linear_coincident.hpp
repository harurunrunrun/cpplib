#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINEAR_COINCIDENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINEAR_COINCIDENT_HPP_INCLUDED

#include "base.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "linear_parallel.hpp"
#include "on_line.hpp"

inline bool coincident(const Line3& first, const Line3& second){
    return parallel(first, second) && on_line(first, second.a);
}

inline bool coincident(const Ray3& first, const Ray3& second){
    return first.origin == second.origin && parallel(first, second) &&
        geometry3d_sign(dot(
            first.through - first.origin,
            second.through - second.origin
        )) > 0;
}

inline bool coincident(const Segment3& first, const Segment3& second){
    return (first.a == second.a && first.b == second.b) ||
        (first.a == second.b && first.b == second.a);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINEAR_COINCIDENT_HPP_INCLUDED
