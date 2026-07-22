#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_SAME_LINE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_SAME_LINE_HPP_INCLUDED

#include "intersect.hpp"
#include "parallel.hpp"

inline bool same_line(const Line& first, const Line& second){
    return parallel(first, second) && intersect(first, second.a);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_SAME_LINE_HPP_INCLUDED
