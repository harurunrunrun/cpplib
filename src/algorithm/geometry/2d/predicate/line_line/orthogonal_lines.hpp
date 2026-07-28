#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_LINE_ORTHOGONAL_LINES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_LINE_ORTHOGONAL_LINES_HPP_INCLUDED

#include <stdexcept>

#include "../../scalar/point/abs.hpp"
#include "../point_point/orthogonal_vectors.hpp"

inline bool orthogonal(const Line& left, const Line& right){
    if(geometry_sign(abs(left.b - left.a)) == 0 ||
       geometry_sign(abs(right.b - right.a)) == 0){
        throw std::invalid_argument("degenerate line");
    }
    return orthogonal(left.b - left.a, right.b - right.a);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_PREDICATE_LINE_LINE_ORTHOGONAL_LINES_HPP_INCLUDED
