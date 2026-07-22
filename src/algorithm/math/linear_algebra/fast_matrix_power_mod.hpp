#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_MATRIX_POWER_MOD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_MATRIX_POWER_MOD_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "fast_matrix_multiply_mod.hpp"
#include "matrix_power_mod.hpp"

namespace math{

inline DynamicModMatrix fast_matrix_power_mod(
    DynamicModMatrix matrix,
    std::uint64_t exponent,
    std::int64_t modulus = 1'000'000'007
){
    using namespace fast_matrix_multiply_mod_internal;
    if(modulus <= 0)[[unlikely]]{
        throw std::invalid_argument(
            "fast_matrix_power_mod requires a positive modulus"
        );
    }
    const std::size_t size = matrix.size();
    for(const auto& row: matrix){
        if(row.size() != size)[[unlikely]]{
            throw std::invalid_argument(
                "fast_matrix_power_mod requires a square matrix"
            );
        }
    }

    const std::size_t area = checked_area(size, size);
    const std::uint64_t unsigned_modulus =
        static_cast<std::uint64_t>(modulus);
    std::vector<std::int64_t> values;
    values.reserve(area);
    for(const auto& row: matrix){
        values.insert(values.end(), row.begin(), row.end());
    }
    std::vector<ModularValue> base = encode(values, unsigned_modulus);
    std::vector<ModularValue> result(area);
    for(std::size_t row = 0; row < size; ++row){
        result[row * size + row] = {
            std::uint64_t{1} % unsigned_modulus,
            unsigned_modulus,
        };
    }

    while(exponent != 0){
        if((exponent & 1U) != 0){
            result = multiply(
                result, size, size, base, size
            );
        }
        exponent >>= 1;
        if(exponent != 0){
            base = multiply(
                base, size, size, base, size
            );
        }
    }

    const std::vector<std::int64_t> result_values = decode(result);
    DynamicModMatrix output(
        size, std::vector<std::int64_t>(size)
    );
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            output[row][column] =
                result_values[row * size + column];
        }
    }
    return output;
}

}  // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_MATRIX_POWER_MOD_HPP_INCLUDED
