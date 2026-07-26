#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_PARALLEL_RAY_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_PARALLEL_RAY_LINE_HPP_INCLUDED

#include "line_ray_parallel_line_ray.hpp"

inline bool parallel(const Ray3& first, const Line3& second){
    return parallel(second, first);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_PARALLEL_RAY_LINE_HPP_INCLUDED