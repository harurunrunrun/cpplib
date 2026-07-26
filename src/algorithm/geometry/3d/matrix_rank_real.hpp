#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MATRIX_RANK_REAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MATRIX_RANK_REAL_HPP_INCLUDED

#include <cstddef>
#include <vector>

#include "gaussian_elimination_real.hpp"

inline std::size_t matrix_rank_real(
    const std::vector<std::vector<long double>>& matrix,
    long double relative_epsilon = GEOMETRY3D_EPS
){
    return gaussian_elimination_real(matrix, relative_epsilon).rank();
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_MATRIX_RANK_REAL_HPP_INCLUDED
