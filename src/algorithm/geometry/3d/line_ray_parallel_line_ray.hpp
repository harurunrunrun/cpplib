#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_PARALLEL_LINE_RAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_PARALLEL_LINE_RAY_HPP_INCLUDED

#include "geometry_primitives.hpp"
#include "line3_direction.hpp"
#include "parallel.hpp"
#include "ray3_direction.hpp"

inline bool parallel(const Line3& first, const Ray3& second){
    return parallel(line3_direction(first), ray3_direction(second));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_RAY_PARALLEL_LINE_RAY_HPP_INCLUDED