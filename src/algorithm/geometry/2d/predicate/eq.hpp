#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_EQ_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_EQ_HPP_INCLUDED

#include "../scalar/geometry_sign.hpp"

inline bool eq(long double first, long double second){
    return geometry_sign(first - second) == 0;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_EQ_HPP_INCLUDED
