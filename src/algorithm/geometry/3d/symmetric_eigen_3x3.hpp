#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>

#include "matrix3.hpp"

struct SymmetricEigen3{
    std::array<long double, 3> eigenvalues{};
    Matrix3 eigenvectors = Matrix3::identity();

    Point3 eigenvector(std::size_t index) const{
        if(index >= 3) throw std::out_of_range("eigenvector index must be below three");
        return {
            eigenvectors[0][index],
            eigenvectors[1][index],
            eigenvectors[2][index],
        };
    }
};

inline SymmetricEigen3 symmetric_eigen_3x3(
    const Matrix3& input,
    long double relative_epsilon = GEOMETRY3D_EPS
){
    if(relative_epsilon < 0 || !std::isfinite(relative_epsilon)){
        throw std::invalid_argument("relative epsilon must be finite and nonnegative");
    }
    long double scale = 0;
    for(std::size_t row = 0; row < 3; ++row){
        for(std::size_t column = 0; column < 3; ++column){
            if(!std::isfinite(input[row][column])){
                throw std::invalid_argument("matrix entries must be finite");
            }
            scale = std::max(scale, std::abs(input[row][column]));
        }
    }
    if(scale == 0) return {};
    for(std::size_t row = 0; row < 3; ++row){
        for(std::size_t column = row + 1; column < 3; ++column){
            if(std::abs(input[row][column] - input[column][row]) >
               relative_epsilon * scale){
                throw std::invalid_argument("matrix must be symmetric");
            }
        }
    }

    Matrix3 matrix;
    for(std::size_t row = 0; row < 3; ++row){
        matrix[row][row] = input[row][row] / scale;
        for(std::size_t column = row + 1; column < 3; ++column){
            matrix[row][column] = matrix[column][row] =
                input[row][column] / (2 * scale) +
                input[column][row] / (2 * scale);
        }
    }
    Matrix3 vectors = Matrix3::identity();
    const long double stopping_epsilon = std::max(
        relative_epsilon,
        16 * std::numeric_limits<long double>::epsilon()
    );
    for(int iteration = 0; iteration < 80; ++iteration){
        std::size_t first = 0;
        std::size_t second = 1;
        long double largest = std::abs(matrix[0][1]);
        for(std::size_t row = 0; row < 3; ++row){
            for(std::size_t column = row + 1; column < 3; ++column){
                if(std::abs(matrix[row][column]) > largest){
                    largest = std::abs(matrix[row][column]);
                    first = row;
                    second = column;
                }
            }
        }
        const long double diagonal_scale = std::max({
            1.0L,
            std::abs(matrix[0][0]),
            std::abs(matrix[1][1]),
            std::abs(matrix[2][2])
        });
        if(largest <= stopping_epsilon * diagonal_scale) break;

        const long double off_diagonal = matrix[first][second];
        const long double tau =
            (matrix[second][second] - matrix[first][first]) /
            (2 * off_diagonal);
        const long double tangent = std::copysign(
            1 / (std::abs(tau) + std::hypot(1.0L, tau)), tau
        );
        const long double cosine = 1 / std::hypot(1.0L, tangent);
        const long double sine = tangent * cosine;
        const long double first_diagonal = matrix[first][first];
        const long double second_diagonal = matrix[second][second];
        matrix[first][first] = first_diagonal - tangent * off_diagonal;
        matrix[second][second] = second_diagonal + tangent * off_diagonal;
        matrix[first][second] = matrix[second][first] = 0;
        for(std::size_t index = 0; index < 3; ++index){
            if(index == first || index == second) continue;
            const long double to_first = matrix[index][first];
            const long double to_second = matrix[index][second];
            matrix[index][first] = matrix[first][index] =
                cosine * to_first - sine * to_second;
            matrix[index][second] = matrix[second][index] =
                sine * to_first + cosine * to_second;
        }
        for(std::size_t row = 0; row < 3; ++row){
            const long double to_first = vectors[row][first];
            const long double to_second = vectors[row][second];
            vectors[row][first] = cosine * to_first - sine * to_second;
            vectors[row][second] = sine * to_first + cosine * to_second;
        }
    }

    std::array<std::size_t, 3> order{{0, 1, 2}};
    std::sort(order.begin(), order.end(), [&](std::size_t left, std::size_t right){
        return matrix[left][left] > matrix[right][right];
    });
    SymmetricEigen3 result;
    for(std::size_t output = 0; output < 3; ++output){
        const std::size_t source = order[output];
        result.eigenvalues[output] = matrix[source][source] * scale;
        if(!std::isfinite(result.eigenvalues[output])){
            throw std::overflow_error("eigenvalue is not representable");
        }
        std::size_t largest_component = 0;
        for(std::size_t row = 1; row < 3; ++row){
            if(std::abs(vectors[row][source]) >
               std::abs(vectors[largest_component][source])){
                largest_component = row;
            }
        }
        const long double direction =
            vectors[largest_component][source] < 0 ? -1 : 1;
        for(std::size_t row = 0; row < 3; ++row){
            result.eigenvectors[row][output] = direction * vectors[row][source];
        }
    }
    return result;
}
