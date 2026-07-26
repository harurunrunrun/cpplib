#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_PARALLEL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_PARALLEL_HPP_INCLUDED

#include "geometry_primitives.hpp"
#include "line3_direction.hpp"
#include "parallel.hpp"

inline bool parallel(const Line3& first, const Line3& second){
    return parallel(line3_direction(first), line3_direction(second));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_PARALLEL_HPP_INCLUDED
