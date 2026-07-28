#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_SCALAR_GAUSSIAN_ELIMINATION_REAL_DETAIL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_SCALAR_GAUSSIAN_ELIMINATION_REAL_DETAIL_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../aggregate/all/geometry_primitives.hpp"
#include "../../type/definition/real_gaussian_elimination_result.hpp"

namespace geometry3d_linear_algebra_detail{

inline std::pair<std::size_t, std::size_t> validate_real_matrix(
    const std::vector<std::vector<long double>>& matrix
){
    const std::size_t rows = matrix.size();
    const std::size_t columns = rows == 0 ? 0 : matrix.front().size();
    for(const auto& row: matrix){
        if(row.size() != columns){
            throw std::invalid_argument("matrix must be rectangular");
        }
        for(long double value: row){
            if(!std::isfinite(value)){
                throw std::invalid_argument("matrix entries must be finite");
            }
        }
    }
    return {rows, columns};
}

inline void validate_relative_epsilon(long double relative_epsilon){
    if(relative_epsilon < 0 || !std::isfinite(relative_epsilon)){
        throw std::invalid_argument("relative epsilon must be finite and nonnegative");
    }
}

inline long double maximum_absolute_entry(
    const std::vector<std::vector<long double>>& matrix
){
    long double scale = 0;
    for(const auto& row: matrix){
        for(long double value: row) scale = std::max(scale, std::abs(value));
    }
    return scale;
}

} // namespace geometry3d_linear_algebra_detail

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DETAIL_SCALAR_GAUSSIAN_ELIMINATION_REAL_DETAIL_HPP_INCLUDED
