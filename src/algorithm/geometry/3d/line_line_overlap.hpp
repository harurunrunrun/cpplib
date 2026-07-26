#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_OVERLAP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_OVERLAP_HPP_INCLUDED

#include <variant>

#include "geometry_primitives.hpp"
#include "line_line_intersection.hpp"

inline bool overlap(const Line3& first, const Line3& second){
    return std::holds_alternative<Line3>(line_line_intersection(first, second));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LINE_LINE_OVERLAP_HPP_INCLUDED
