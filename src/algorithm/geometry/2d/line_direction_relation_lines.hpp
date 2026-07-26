#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_DIRECTION_RELATION_LINES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_DIRECTION_RELATION_LINES_HPP_INCLUDED

#include "line_direction_relation_result.hpp"
#include "orthogonal_lines.hpp"
#include "parallel_lines.hpp"

inline LineDirectionRelation line_direction_relation(
    const Line& first,
    const Line& second
){
    if(parallel(first, second)) return LineDirectionRelation::PARALLEL;
    if(orthogonal(first, second)) return LineDirectionRelation::ORTHOGONAL;
    return LineDirectionRelation::NEITHER;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_LINE_DIRECTION_RELATION_LINES_HPP_INCLUDED
