#ifndef CPPLIB_SRC_ALGORITHM_MATH_FAST_MATRIX_POWER_MOD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_FAST_MATRIX_POWER_MOD_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include "fast_matrix_multiply.hpp"
#include "matrix_power_mod.hpp"

namespace math{

namespace fast_matrix_power_mod_internal{

class ModularValue{
    std::uint64_t value_ = 0;
    std::uint64_t modulus_ = 0;

    static std::uint64_t common_modulus(
        const ModularValue& left,
        const ModularValue& right
    ){
        if(left.modulus_ == 0) return right.modulus_;
        if(right.modulus_ == 0) return left.modulus_;
        if(left.modulus_ != right.modulus_)[[unlikely]]{
            throw std::logic_error(
                "fast_matrix_power_mod mixed different moduli"
            );
        }
        return left.modulus_;
    }

public:
    ModularValue() = default;

    ModularValue(std::uint64_t value, std::uint64_t modulus):
        value_(modulus == 0 ? 0 : value % modulus),
        modulus_(modulus){}

    [[nodiscard]] std::uint64_t value() const{
        return value_;
    }

    friend ModularValue operator+(
        const ModularValue& left,
        const ModularValue& right
    ){
        const std::uint64_t modulus = common_modulus(left, right);
        if(modulus == 0) return {};
        const std::uint64_t sum = left.value_ + right.value_;
        return {
            sum >= modulus ? sum - modulus : sum,
            modulus,
        };
    }

    friend ModularValue operator-(
        const ModularValue& left,
        const ModularValue& right
    ){
        const std::uint64_t modulus = common_modulus(left, right);
        if(modulus == 0) return {};
        return {
            left.value_ >= right.value_
                ? left.value_ - right.value_
                : modulus - (right.value_ - left.value_),
            modulus,
        };
    }

    friend ModularValue operator*(
        const ModularValue& left,
        const ModularValue& right
    ){
        const std::uint64_t modulus = common_modulus(left, right);
        if(modulus == 0) return {};
        const auto product =
            static_cast<unsigned __int128>(left.value_) * right.value_;
        return {
            static_cast<std::uint64_t>(product % modulus),
            modulus,
        };
    }

    ModularValue& operator+=(const ModularValue& other){
        *this = *this + other;
        return *this;
    }

    ModularValue& operator-=(const ModularValue& other){
        *this = *this - other;
        return *this;
    }
};

inline std::size_t checked_square(std::size_t size){
    if(size != 0
        && size > std::numeric_limits<std::size_t>::max() / size)[[unlikely]]{
        throw std::length_error(
            "fast_matrix_power_mod matrix size is too large"
        );
    }
    return size * size;
}

inline std::vector<ModularValue> multiply(
    const std::vector<ModularValue>& left,
    const std::vector<ModularValue>& right,
    std::size_t size
){
    return fast_matrix_multiply(
        left, size, size, right, size
    );
}

}  // namespace fast_matrix_power_mod_internal

inline DynamicModMatrix fast_matrix_power_mod(
    DynamicModMatrix matrix,
    std::uint64_t exponent,
    std::int64_t modulus = 1'000'000'007
){
    using fast_matrix_power_mod_internal::ModularValue;
    if(modulus <= 0)[[unlikely]]{
        throw std::invalid_argument(
            "fast_matrix_power_mod requires a positive modulus"
        );
    }
    const std::size_t size = matrix.size();
    for(auto& row: matrix){
        if(row.size() != size)[[unlikely]]{
            throw std::invalid_argument(
                "fast_matrix_power_mod requires a square matrix"
            );
        }
        for(std::int64_t& value: row){
            value %= modulus;
            if(value < 0) value += modulus;
        }
    }

    const std::size_t area =
        fast_matrix_power_mod_internal::checked_square(size);
    const std::uint64_t unsigned_modulus =
        static_cast<std::uint64_t>(modulus);
    std::vector<ModularValue> base(area);
    std::vector<ModularValue> result(area);
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            base[row * size + column] = {
                static_cast<std::uint64_t>(matrix[row][column]),
                unsigned_modulus,
            };
        }
        result[row * size + row] = {
            std::uint64_t{1} % unsigned_modulus,
            unsigned_modulus,
        };
    }

    while(exponent != 0){
        if((exponent & 1U) != 0){
            result = fast_matrix_power_mod_internal::multiply(
                result, base, size
            );
        }
        exponent >>= 1;
        if(exponent != 0){
            base = fast_matrix_power_mod_internal::multiply(
                base, base, size
            );
        }
    }

    DynamicModMatrix output(
        size, std::vector<std::int64_t>(size)
    );
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            output[row][column] = static_cast<std::int64_t>(
                result[row * size + column].value()
            );
        }
    }
    return output;
}

}  // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_FAST_MATRIX_POWER_MOD_HPP_INCLUDED
