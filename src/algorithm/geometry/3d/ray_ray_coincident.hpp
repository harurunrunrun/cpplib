#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_COINCIDENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_COINCIDENT_HPP_INCLUDED

#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "geometry_primitives.hpp"
#include "ray_ray_parallel.hpp"

inline bool coincident(const Ray3& first, const Ray3& second){
    return parallel(first, second) && first.origin == second.origin &&
        geometry3d_sign(dot(
            first.through - first.origin,
            second.through - second.origin
        )) > 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_COINCIDENT_HPP_INCLUDED
