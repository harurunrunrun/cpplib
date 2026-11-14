#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_MATRIX_OPERATIONS_MATRIX_POWER_MOD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_MATRIX_OPERATIONS_MATRIX_POWER_MOD_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
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
    // Entries are in [0, modulus). Accumulate each row without reducing every term:
    // in uint64 (reduced every `block` terms) when the products are small enough,
    // otherwise in unsigned __int128 when the whole row sum fits.
    using u64 = std::uint64_t;
    using u128 = unsigned __int128;
    const std::size_t n = lhs.size();
    DynamicModMatrix result(n, std::vector<std::int64_t>(n));
    const u64 maximum_value = static_cast<u64>(modulus) - 1;
    const u64 umodulus = static_cast<u64>(modulus);
    u64 block = std::numeric_limits<u64>::max();
    if(maximum_value != 0 && maximum_value <= std::numeric_limits<std::uint32_t>::max()){
        block = (std::numeric_limits<u64>::max() - maximum_value) /
            (maximum_value * maximum_value);
    }else if(maximum_value != 0){
        block = 0;
    }
    if(block >= 8){
        std::vector<u64> accumulator(n);
        for(std::size_t i = 0; i < n; ++i){
            std::fill(accumulator.begin(), accumulator.end(), 0);
            u64 pending = 0;
            for(std::size_t k = 0; k < n; ++k){
                if(lhs[i][k] == 0) continue;
                if(pending == block){
                    for(u64& value: accumulator) value %= umodulus;
                    pending = 0;
                }
                ++pending;
                const u64 coefficient = static_cast<u64>(lhs[i][k]);
                const std::vector<std::int64_t>& row = rhs[k];
                for(std::size_t j = 0; j < n; ++j){
                    accumulator[j] += coefficient * static_cast<u64>(row[j]);
                }
            }
            for(std::size_t j = 0; j < n; ++j){
                result[i][j] = static_cast<std::int64_t>(accumulator[j] % umodulus);
            }
        }
        return result;
    }
    const u128 square = static_cast<u128>(maximum_value) * maximum_value;
    if(n == 0 || square <= std::numeric_limits<u128>::max() / n){
        std::vector<u128> accumulator(n);
        for(std::size_t i = 0; i < n; ++i){
            std::fill(accumulator.begin(), accumulator.end(), static_cast<u128>(0));
            for(std::size_t k = 0; k < n; ++k){
                if(lhs[i][k] == 0) continue;
                const u128 coefficient = static_cast<u64>(lhs[i][k]);
                const std::vector<std::int64_t>& row = rhs[k];
                for(std::size_t j = 0; j < n; ++j){
                    accumulator[j] += coefficient * static_cast<u64>(row[j]);
                }
            }
            for(std::size_t j = 0; j < n; ++j){
                result[i][j] = static_cast<std::int64_t>(accumulator[j] % umodulus);
            }
        }
        return result;
    }
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

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_MATRIX_OPERATIONS_MATRIX_POWER_MOD_HPP_INCLUDED
