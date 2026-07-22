#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_MATRIX_POWER_ENTRY_QUERIES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_MATRIX_POWER_ENTRY_QUERIES_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace math{

template<std::uint32_t Modulus = 1'000'000'007>
class MatrixPowerEntryQueries{
    static_assert(Modulus > 0, "MatrixPowerEntryQueries requires a positive modulus");

public:
    using value_type = std::uint32_t;
    using input_matrix_type = std::vector<std::vector<std::int64_t>>;

    explicit MatrixPowerEntryQueries(const input_matrix_type& matrix):
        dimension_(matrix.size()){
        if(dimension_ == 0)[[unlikely]]{
            throw std::invalid_argument("MatrixPowerEntryQueries requires a non-empty matrix");
        }
        if(dimension_ > std::numeric_limits<std::size_t>::max() / dimension_)[[unlikely]]{
            throw std::length_error("MatrixPowerEntryQueries matrix is too large");
        }
        for(const auto& row: matrix){
            if(row.size() != dimension_)[[unlikely]]{
                throw std::invalid_argument("MatrixPowerEntryQueries requires a square matrix");
            }
        }

        const wide_type maximum_value = static_cast<wide_type>(Modulus) - 1;
        use_uint64_accumulator_ =
            static_cast<wide_type>(dimension_) * maximum_value * maximum_value <=
            std::numeric_limits<std::uint64_t>::max();

        current_.resize(dimension_);
        next_.resize(dimension_);
        if(use_uint64_accumulator_){
            accumulator64_.resize(dimension_);
        }else{
            accumulator_wide_.resize(dimension_);
        }

        const std::size_t element_count = dimension_ * dimension_;
        powers_[0].resize(element_count);
        const std::int64_t signed_modulus = static_cast<std::int64_t>(Modulus);
        for(std::size_t row = 0; row < dimension_; ++row){
            for(std::size_t column = 0; column < dimension_; ++column){
                std::int64_t value = matrix[row][column] % signed_modulus;
                if(value < 0) value += signed_modulus;
                powers_[0][row * dimension_ + column] = static_cast<value_type>(value);
            }
        }
        for(std::size_t bit = 1; bit < exponent_bits; ++bit){
            powers_[bit] = multiply_matrices(powers_[bit - 1], powers_[bit - 1]);
        }
    }

    [[nodiscard]] value_type entry(
        std::size_t row,
        std::size_t column,
        std::uint64_t exponent
    ){
        if(row >= dimension_ || column >= dimension_)[[unlikely]]{
            throw std::out_of_range("MatrixPowerEntryQueries query index is out of range");
        }
        if(exponent == 0){
            return row == column ? static_cast<value_type>(1 % Modulus) : 0;
        }

        bool initialized = false;
        std::size_t bit = 0;
        while(exponent != 0){
            if(exponent & 1U){
                if(!initialized){
                    std::copy_n(
                        powers_[bit].begin() + static_cast<std::ptrdiff_t>(row * dimension_),
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
        return current_[column];
    }

private:
    using wide_type = unsigned __int128;
    using matrix_type = std::vector<value_type>;
    static constexpr std::size_t exponent_bits =
        std::numeric_limits<std::uint64_t>::digits;

    matrix_type multiply_matrices(const matrix_type& lhs, const matrix_type& rhs){
        matrix_type result(dimension_ * dimension_);
        for(std::size_t row = 0; row < dimension_; ++row){
            if(use_uint64_accumulator_){
                std::fill(accumulator64_.begin(), accumulator64_.end(), 0);
                for(std::size_t middle = 0; middle < dimension_; ++middle){
                    const value_type coefficient = lhs[row * dimension_ + middle];
                    if(coefficient == 0) continue;
                    const std::size_t rhs_offset = middle * dimension_;
                    for(std::size_t column = 0; column < dimension_; ++column){
                        accumulator64_[column] +=
                            static_cast<std::uint64_t>(coefficient) * rhs[rhs_offset + column];
                    }
                }
                for(std::size_t column = 0; column < dimension_; ++column){
                    result[row * dimension_ + column] = static_cast<value_type>(
                        accumulator64_[column] % Modulus
                    );
                }
            }else{
                std::fill(
                    accumulator_wide_.begin(),
                    accumulator_wide_.end(),
                    static_cast<wide_type>(0)
                );
                for(std::size_t middle = 0; middle < dimension_; ++middle){
                    const value_type coefficient = lhs[row * dimension_ + middle];
                    if(coefficient == 0) continue;
                    const std::size_t rhs_offset = middle * dimension_;
                    for(std::size_t column = 0; column < dimension_; ++column){
                        accumulator_wide_[column] +=
                            static_cast<wide_type>(coefficient) * rhs[rhs_offset + column];
                    }
                }
                for(std::size_t column = 0; column < dimension_; ++column){
                    result[row * dimension_ + column] = static_cast<value_type>(
                        accumulator_wide_[column] % Modulus
                    );
                }
            }
        }
        return result;
    }

    void multiply_row(const matrix_type& matrix){
        if(use_uint64_accumulator_){
            std::fill(accumulator64_.begin(), accumulator64_.end(), 0);
            for(std::size_t middle = 0; middle < dimension_; ++middle){
                const value_type coefficient = current_[middle];
                if(coefficient == 0) continue;
                const std::size_t matrix_offset = middle * dimension_;
                for(std::size_t column = 0; column < dimension_; ++column){
                    accumulator64_[column] +=
                        static_cast<std::uint64_t>(coefficient) * matrix[matrix_offset + column];
                }
            }
            for(std::size_t column = 0; column < dimension_; ++column){
                next_[column] = static_cast<value_type>(accumulator64_[column] % Modulus);
            }
        }else{
            std::fill(
                accumulator_wide_.begin(),
                accumulator_wide_.end(),
                static_cast<wide_type>(0)
            );
            for(std::size_t middle = 0; middle < dimension_; ++middle){
                const value_type coefficient = current_[middle];
                if(coefficient == 0) continue;
                const std::size_t matrix_offset = middle * dimension_;
                for(std::size_t column = 0; column < dimension_; ++column){
                    accumulator_wide_[column] +=
                        static_cast<wide_type>(coefficient) * matrix[matrix_offset + column];
                }
            }
            for(std::size_t column = 0; column < dimension_; ++column){
                next_[column] = static_cast<value_type>(accumulator_wide_[column] % Modulus);
            }
        }
    }

    std::size_t dimension_;
    bool use_uint64_accumulator_ = false;
    std::array<matrix_type, exponent_bits> powers_;
    std::vector<value_type> current_;
    std::vector<value_type> next_;
    std::vector<std::uint64_t> accumulator64_;
    std::vector<wide_type> accumulator_wide_;
};

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_MATRIX_POWER_ENTRY_QUERIES_HPP_INCLUDED
