#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"

struct RealGaussianEliminationResult{
    std::vector<std::vector<long double>> reduced_row_echelon_form;
    std::vector<std::size_t> pivot_columns;

    std::size_t rank() const noexcept{ return pivot_columns.size(); }
};

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
