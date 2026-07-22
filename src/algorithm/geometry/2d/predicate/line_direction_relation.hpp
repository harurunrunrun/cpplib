#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_DIRECTION_RELATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_DIRECTION_RELATION_HPP_INCLUDED

#include "orthogonal.hpp"
#include "parallel.hpp"

enum class LineDirectionRelation : int{
    NEITHER = 0,
    ORTHOGONAL = 1,
    PARALLEL = 2,
};

inline LineDirectionRelation line_direction_relation(
    const Line& first,
    const Line& second
){
    if(parallel(first, second)) return LineDirectionRelation::PARALLEL;
    if(orthogonal(first, second)) return LineDirectionRelation::ORTHOGONAL;
    return LineDirectionRelation::NEITHER;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_DIRECTION_RELATION_HPP_INCLUDED
