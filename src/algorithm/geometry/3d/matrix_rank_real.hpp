#pragma once

#include <cstddef>
#include <vector>

#include "gaussian_elimination_real.hpp"

inline std::size_t matrix_rank_real(
    const std::vector<std::vector<long double>>& matrix,
    long double relative_epsilon = GEOMETRY3D_EPS
){
    return gaussian_elimination_real(matrix, relative_epsilon).rank();
}
