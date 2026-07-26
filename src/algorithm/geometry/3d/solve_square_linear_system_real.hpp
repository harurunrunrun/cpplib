#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SOLVE_SQUARE_LINEAR_SYSTEM_REAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SOLVE_SQUARE_LINEAR_SYSTEM_REAL_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "gaussian_elimination_real_detail.hpp"

inline std::optional<std::vector<long double>> solve_square_linear_system_real(
    const std::vector<std::vector<long double>>& matrix,
    const std::vector<long double>& right_hand_side,
    long double relative_epsilon = GEOMETRY3D_EPS
){
    using namespace geometry3d_linear_algebra_detail;
    const auto [rows, columns] = validate_real_matrix(matrix);
    validate_relative_epsilon(relative_epsilon);
    if(rows != columns || right_hand_side.size() != rows){
        throw std::invalid_argument("a square matrix and matching right hand side are required");
    }
    for(long double value: right_hand_side){
        if(!std::isfinite(value)){
            throw std::invalid_argument("right hand side entries must be finite");
        }
    }
    if(rows == 0) return std::vector<long double>{};

    const long double coefficient_scale = maximum_absolute_entry(matrix);
    if(coefficient_scale == 0) return std::nullopt;
    long double right_scale = 0;
    for(long double value: right_hand_side){
        right_scale = std::max(right_scale, std::abs(value));
    }
    if(right_scale == 0) right_scale = 1;

    std::vector<std::vector<long double>> augmented(
        rows, std::vector<long double>(columns + 1)
    );
    for(std::size_t row = 0; row < rows; ++row){
        for(std::size_t column = 0; column < columns; ++column){
            augmented[row][column] = matrix[row][column] / coefficient_scale;
        }
        augmented[row][columns] = right_hand_side[row] / right_scale;
    }

    for(std::size_t column = 0; column < columns; ++column){
        std::size_t pivot = column;
        for(std::size_t row = column + 1; row < rows; ++row){
            if(std::abs(augmented[row][column]) >
               std::abs(augmented[pivot][column])) pivot = row;
        }
        if(std::abs(augmented[pivot][column]) <= relative_epsilon){
            return std::nullopt;
        }
        if(pivot != column) std::swap(augmented[pivot], augmented[column]);
        for(std::size_t row = column + 1; row < rows; ++row){
            const long double factor =
                augmented[row][column] / augmented[column][column];
            augmented[row][column] = 0;
            for(std::size_t current = column + 1; current <= columns; ++current){
                augmented[row][current] -= factor * augmented[column][current];
            }
        }
    }

    std::vector<long double> normalized_solution(rows);
    for(std::size_t reverse = rows; reverse > 0; --reverse){
        const std::size_t row = reverse - 1;
        long double value = augmented[row][columns];
        for(std::size_t column = row + 1; column < columns; ++column){
            value -= augmented[row][column] * normalized_solution[column];
        }
        normalized_solution[row] = value / augmented[row][row];
    }

    std::vector<long double> solution(rows);
    for(std::size_t index = 0; index < rows; ++index){
        solution[index] = normalized_solution[index] *
            (right_scale / coefficient_scale);
        if(!std::isfinite(solution[index])){
            throw std::overflow_error("linear-system solution is not representable");
        }
    }
    return solution;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_SOLVE_SQUARE_LINEAR_SYSTEM_REAL_HPP_INCLUDED
