#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ORTHOGONAL_LINES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ORTHOGONAL_LINES_HPP_INCLUDED

#include <stdexcept>

#include "abs.hpp"
#include "orthogonal_vectors.hpp"

inline bool orthogonal(const Line& left, const Line& right){
    if(geometry_sign(abs(left.b - left.a)) == 0 ||
       geometry_sign(abs(right.b - right.a)) == 0){
        throw std::invalid_argument("degenerate line");
    }
    return orthogonal(left.b - left.a, right.b - right.a);
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_ORTHOGONAL_LINES_HPP_INCLUDED
