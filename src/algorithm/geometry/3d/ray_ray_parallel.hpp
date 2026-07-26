#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_PARALLEL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_PARALLEL_HPP_INCLUDED

#include "geometry_primitives.hpp"
#include "parallel.hpp"
#include "ray3_direction.hpp"

inline bool parallel(const Ray3& first, const Ray3& second){
    return parallel(ray3_direction(first), ray3_direction(second));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RAY_RAY_PARALLEL_HPP_INCLUDED
