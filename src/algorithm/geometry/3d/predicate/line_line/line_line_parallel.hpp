#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_LINE_LINE_LINE_PARALLEL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_LINE_LINE_LINE_PARALLEL_HPP_INCLUDED

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../point/line/line3_direction.hpp"
#include "../point_point/parallel.hpp"

inline bool parallel(const Line3& first, const Line3& second){
    return parallel(line3_direction(first), line3_direction(second));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_LINE_LINE_LINE_PARALLEL_HPP_INCLUDED
