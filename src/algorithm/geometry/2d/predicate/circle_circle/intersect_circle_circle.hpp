#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_CIRCLE_CIRCLE_INTERSECT_CIRCLE_CIRCLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_CIRCLE_CIRCLE_INTERSECT_CIRCLE_CIRCLE_HPP_INCLUDED

#include "../../aggregate/all/circle_relation.hpp"

inline bool intersect_circle_circle(const Circle& first, const Circle& second){
    const int relation = circle_relation(first, second);
    return relation != CIRCLE_INSIDE && relation != CIRCLE_SEPARATE;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_CIRCLE_CIRCLE_INTERSECT_CIRCLE_CIRCLE_HPP_INCLUDED
