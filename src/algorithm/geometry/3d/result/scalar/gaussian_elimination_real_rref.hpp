#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_SCALAR_GAUSSIAN_ELIMINATION_REAL_RREF_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_SCALAR_GAUSSIAN_ELIMINATION_REAL_RREF_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>

#include "../../detail/scalar/gaussian_elimination_real_detail.hpp"
#include "../../type/definition/real_gaussian_elimination_result.hpp"

inline RealGaussianEliminationResult gaussian_elimination_real(
    const std::vector<std::vector<long double>>& input,
    long double relative_epsilon = GEOMETRY3D_EPS
){
    using namespace geometry3d_linear_algebra_detail;
    const auto [rows, columns] = validate_real_matrix(input);
    validate_relative_epsilon(relative_epsilon);

    RealGaussianEliminationResult result;
    result.reduced_row_echelon_form = input;
    const long double scale = maximum_absolute_entry(input);
    if(scale == 0) return result;
    for(auto& row: result.reduced_row_echelon_form){
        for(long double& value: row) value /= scale;
    }

    std::size_t pivot_row = 0;
    for(std::size_t column = 0; column < columns && pivot_row < rows; ++column){
        std::size_t pivot = pivot_row;
        for(std::size_t row = pivot_row + 1; row < rows; ++row){
            if(std::abs(result.reduced_row_echelon_form[row][column]) >
               std::abs(result.reduced_row_echelon_form[pivot][column])){
                pivot = row;
            }
        }
        const long double pivot_value =
            result.reduced_row_echelon_form[pivot][column];
        if(std::abs(pivot_value) <= relative_epsilon) continue;
        if(pivot != pivot_row){
            std::swap(result.reduced_row_echelon_form[pivot],
                      result.reduced_row_echelon_form[pivot_row]);
        }

        auto& normalized = result.reduced_row_echelon_form[pivot_row];
        const long double divisor = normalized[column];
        for(std::size_t current = column; current < columns; ++current){
            normalized[current] /= divisor;
        }
        normalized[column] = 1;

        for(std::size_t row = 0; row < rows; ++row){
            if(row == pivot_row) continue;
            auto& current_row = result.reduced_row_echelon_form[row];
            const long double factor = current_row[column];
            if(factor == 0) continue;
            current_row[column] = 0;
            for(std::size_t current = column + 1; current < columns; ++current){
                current_row[current] -= factor * normalized[current];
            }
        }
        result.pivot_columns.push_back(column);
        ++pivot_row;
    }

    for(auto& row: result.reduced_row_echelon_form){
        for(long double& value: row){
            if(std::abs(value) <= relative_epsilon) value = 0;
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_RESULT_SCALAR_GAUSSIAN_ELIMINATION_REAL_RREF_HPP_INCLUDED
