#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_COINCIDENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_COINCIDENT_HPP_INCLUDED

#include "geometry_primitives.hpp"
#include "line_line_parallel.hpp"
#include "on_line.hpp"

inline bool coincident(const Line3& first, const Line3& second){
    return parallel(first, second) && on_line(first, second.a);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_COINCIDENT_HPP_INCLUDED
