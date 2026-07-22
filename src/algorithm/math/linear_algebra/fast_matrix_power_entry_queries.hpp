#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_MATRIX_POWER_ENTRY_QUERIES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_MATRIX_POWER_ENTRY_QUERIES_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include "fast_matrix_multiply_mod.hpp"

namespace math{

class FastMatrixPowerEntryQueries{
public:
    using value_type = std::int64_t;
    using input_matrix_type = std::vector<std::vector<std::int64_t>>;

    explicit FastMatrixPowerEntryQueries(
        const input_matrix_type& matrix,
        std::int64_t modulus = 1'000'000'007
    ):
        dimension_(matrix.size()),
        modulus_(modulus){
        using namespace fast_matrix_multiply_mod_internal;
        if(modulus_ <= 0)[[unlikely]]{
            throw std::invalid_argument(
                "FastMatrixPowerEntryQueries requires a positive modulus"
            );
        }
        const std::size_t area = checked_area(dimension_, dimension_);
        std::vector<std::int64_t> values;
        values.reserve(area);
        for(const auto& row: matrix){
            if(row.size() != dimension_)[[unlikely]]{
                throw std::invalid_argument(
                    "FastMatrixPowerEntryQueries requires a square matrix"
                );
            }
            values.insert(values.end(), row.begin(), row.end());
        }

        const std::uint64_t unsigned_modulus =
            static_cast<std::uint64_t>(modulus_);
        powers_[0] = encode(values, unsigned_modulus);
        if(dimension_ != 0){
            for(std::size_t bit = 1; bit < exponent_bits; ++bit){
                powers_[bit] = multiply(
                    powers_[bit - 1], dimension_, dimension_,
                    powers_[bit - 1], dimension_
                );
            }
        }
        current_.resize(dimension_);
        next_.resize(dimension_);
    }

    [[nodiscard]] value_type entry(
        std::size_t row,
        std::size_t column,
        std::uint64_t exponent
    ){
        if(row >= dimension_ || column >= dimension_)[[unlikely]]{
            throw std::out_of_range(
                "FastMatrixPowerEntryQueries query index is out of range"
            );
        }
        if(exponent == 0){
            return row == column ? static_cast<value_type>(1 % modulus_) : 0;
        }

        bool initialized = false;
        std::size_t bit = 0;
        while(exponent != 0){
            if((exponent & 1U) != 0){
                if(!initialized){
                    std::copy_n(
                        powers_[bit].begin()
                            + static_cast<std::ptrdiff_t>(row * dimension_),
                        dimension_,
                        current_.begin()
                    );
                    initialized = true;
                }else{
                    multiply_row(powers_[bit]);
                    current_.swap(next_);
                }
            }
            exponent >>= 1;
            ++bit;
        }
        return static_cast<value_type>(current_[column].value());
    }

private:
    using modular_value = fast_matrix_multiply_mod_internal::ModularValue;
    using matrix_type = std::vector<modular_value>;
    static constexpr std::size_t exponent_bits =
        std::numeric_limits<std::uint64_t>::digits;

    void multiply_row(const matrix_type& matrix){
        const modular_value zero(
            0, static_cast<std::uint64_t>(modulus_)
        );
        std::fill(next_.begin(), next_.end(), zero);
        for(std::size_t middle = 0; middle < dimension_; ++middle){
            const modular_value coefficient = current_[middle];
            if(coefficient.value() == 0) continue;
            const std::size_t matrix_offset = middle * dimension_;
            for(std::size_t column = 0; column < dimension_; ++column){
                next_[column] +=
                    coefficient * matrix[matrix_offset + column];
            }
        }
    }

    std::size_t dimension_ = 0;
    std::int64_t modulus_ = 1;
    std::array<matrix_type, exponent_bits> powers_;
    std::vector<modular_value> current_;
    std::vector<modular_value> next_;
};

}  // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_MATRIX_POWER_ENTRY_QUERIES_HPP_INCLUDED
