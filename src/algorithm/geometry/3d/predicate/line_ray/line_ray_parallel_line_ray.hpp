#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_RAY_LINE_RAY_PARALLEL_LINE_RAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_RAY_LINE_RAY_PARALLEL_LINE_RAY_HPP_INCLUDED

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../point/line/line3_direction.hpp"
#include "../point_point/parallel.hpp"
#include "../../point/ray/ray3_direction.hpp"

inline bool parallel(const Line3& first, const Ray3& second){
    return parallel(line3_direction(first), ray3_direction(second));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_RAY_LINE_RAY_PARALLEL_LINE_RAY_HPP_INCLUDED
