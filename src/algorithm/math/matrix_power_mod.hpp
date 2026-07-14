#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace math{

using DynamicModMatrix = std::vector<std::vector<std::int64_t>>;

namespace matrix_power_mod_internal{

inline DynamicModMatrix multiply(
    const DynamicModMatrix& lhs,
    const DynamicModMatrix& rhs,
    std::int64_t modulus
){
    const std::size_t n = lhs.size();
    DynamicModMatrix result(n, std::vector<std::int64_t>(n));
    for(std::size_t i = 0; i < n; ++i){
        for(std::size_t k = 0; k < n; ++k){
            if(lhs[i][k] == 0) continue;
            for(std::size_t j = 0; j < n; ++j){
                result[i][j] = static_cast<std::int64_t>(
                    (result[i][j] + static_cast<__int128>(lhs[i][k]) * rhs[k][j]) %
                    modulus
                );
            }
        }
    }
    return result;
}

} // namespace matrix_power_mod_internal

inline DynamicModMatrix matrix_power_mod(
    DynamicModMatrix matrix,
    std::uint64_t exponent,
    std::int64_t modulus = 1'000'000'007
){
    if(modulus <= 0)[[unlikely]]{
        throw std::invalid_argument("matrix_power_mod requires a positive modulus");
    }
    const std::size_t n = matrix.size();
    for(auto& row: matrix){
        if(row.size() != n)[[unlikely]]{
            throw std::invalid_argument("matrix_power_mod requires a square matrix");
        }
        for(auto& value: row){
            value %= modulus;
            if(value < 0) value += modulus;
        }
    }
    DynamicModMatrix result(n, std::vector<std::int64_t>(n));
    for(std::size_t i = 0; i < n; ++i) result[i][i] = 1 % modulus;
    while(exponent != 0){
        if(exponent & 1){
            result = matrix_power_mod_internal::multiply(result, matrix, modulus);
        }
        exponent >>= 1;
        if(exponent != 0){
            matrix = matrix_power_mod_internal::multiply(matrix, matrix, modulus);
        }
    }
    return result;
}

} // namespace math
