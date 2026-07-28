#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_LINE_LINE_LINE_OVERLAP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_LINE_LINE_LINE_OVERLAP_HPP_INCLUDED

#include <variant>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../result/line_line/line_line_intersection.hpp"

inline bool overlap(const Line3& first, const Line3& second){
    return std::holds_alternative<Line3>(line_line_intersection(first, second));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_LINE_LINE_LINE_LINE_OVERLAP_HPP_INCLUDED
