#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LEAST_SQUARES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LEAST_SQUARES_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>

#include "gaussian_elimination_real.hpp"

inline std::optional<std::vector<long double>> least_squares(
    const std::vector<std::vector<long double>>& matrix,
    const std::vector<long double>& right_hand_side,
    long double relative_epsilon = GEOMETRY3D_EPS
){
    using namespace geometry3d_linear_algebra_detail;
    const auto [rows, columns] = validate_real_matrix(matrix);
    validate_relative_epsilon(relative_epsilon);
    if(right_hand_side.size() != rows){
        throw std::invalid_argument("right hand side size must match matrix rows");
    }
    for(long double value: right_hand_side){
        if(!std::isfinite(value)){
            throw std::invalid_argument("right hand side entries must be finite");
        }
    }
    if(columns == 0) return std::vector<long double>{};
    if(rows < columns) return std::nullopt;

    std::vector<long double> column_scales(columns);
    std::vector<std::vector<long double>> columns_data(
        columns, std::vector<long double>(rows)
    );
    for(std::size_t column = 0; column < columns; ++column){
        long double maximum = 0;
        for(std::size_t row = 0; row < rows; ++row){
            maximum = std::max(maximum, std::abs(matrix[row][column]));
        }
        if(maximum == 0) return std::nullopt;
        long double squared_norm = 0;
        for(std::size_t row = 0; row < rows; ++row){
            const long double value = matrix[row][column] / maximum;
            columns_data[column][row] = value;
            squared_norm += value * value;
        }
        column_scales[column] = maximum * std::sqrt(squared_norm);
        if(!std::isfinite(column_scales[column]) || column_scales[column] == 0){
            throw std::overflow_error("column norm is not representable");
        }
        for(long double& value: columns_data[column]){
            value = value * maximum / column_scales[column];
        }
    }

    std::vector<std::vector<long double>> orthonormal_columns;
    orthonormal_columns.reserve(columns);
    std::vector<std::vector<long double>> upper(
        columns, std::vector<long double>(columns)
    );
    for(std::size_t column = 0; column < columns; ++column){
        std::vector<long double> value = columns_data[column];
        for(int pass = 0; pass < 2; ++pass){
            for(std::size_t previous = 0; previous < column; ++previous){
                long double coefficient = 0;
                for(std::size_t row = 0; row < rows; ++row){
                    coefficient += orthonormal_columns[previous][row] * value[row];
                }
                upper[previous][column] += coefficient;
                for(std::size_t row = 0; row < rows; ++row){
                    value[row] -= coefficient * orthonormal_columns[previous][row];
                }
            }
        }
        long double squared_norm = 0;
        for(long double entry: value) squared_norm += entry * entry;
        const long double length = std::sqrt(std::max(0.0L, squared_norm));
        if(length <= relative_epsilon) return std::nullopt;
        upper[column][column] = length;
        for(long double& entry: value) entry /= length;
        orthonormal_columns.push_back(std::move(value));
    }

    long double right_scale = 0;
    for(long double value: right_hand_side){
        right_scale = std::max(right_scale, std::abs(value));
    }
    if(right_scale == 0) return std::vector<long double>(columns, 0);
    std::vector<long double> transformed(columns);
    for(std::size_t column = 0; column < columns; ++column){
        for(std::size_t row = 0; row < rows; ++row){
            transformed[column] += orthonormal_columns[column][row] *
                (right_hand_side[row] / right_scale);
        }
    }

    std::vector<long double> normalized_solution(columns);
    for(std::size_t reverse = columns; reverse > 0; --reverse){
        const std::size_t row = reverse - 1;
        long double value = transformed[row];
        for(std::size_t column = row + 1; column < columns; ++column){
            value -= upper[row][column] * normalized_solution[column];
        }
        normalized_solution[row] = value / upper[row][row];
    }

    std::vector<long double> solution(columns);
    for(std::size_t column = 0; column < columns; ++column){
        solution[column] = normalized_solution[column] *
            (right_scale / column_scales[column]);
        if(!std::isfinite(solution[column])){
            throw std::overflow_error("least-squares solution is not representable");
        }
    }
    return solution;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_LEAST_SQUARES_HPP_INCLUDED
