#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_MATRIX_MULTIPLY_MOD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_MATRIX_MULTIPLY_MOD_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include "fast_matrix_multiply.hpp"

namespace math{

namespace fast_matrix_multiply_mod_internal{

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
                "fast_matrix_multiply_mod mixed different moduli"
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

inline std::size_t checked_area(std::size_t rows, std::size_t columns){
    if(rows != 0
        && columns > std::numeric_limits<std::size_t>::max() / rows)[[unlikely]]{
        throw std::length_error(
            "fast_matrix_multiply_mod matrix size is too large"
        );
    }
    return rows * columns;
}

inline std::uint64_t normalize(
    std::int64_t value,
    std::int64_t modulus
){
    value %= modulus;
    if(value < 0) value += modulus;
    return static_cast<std::uint64_t>(value);
}

inline std::vector<ModularValue> encode(
    const std::vector<std::int64_t>& values,
    std::uint64_t modulus
){
    std::vector<ModularValue> result;
    result.reserve(values.size());
    const std::int64_t signed_modulus =
        static_cast<std::int64_t>(modulus);
    for(const std::int64_t value: values){
        result.emplace_back(normalize(value, signed_modulus), modulus);
    }
    return result;
}

inline std::vector<std::int64_t> decode(
    const std::vector<ModularValue>& values
){
    std::vector<std::int64_t> result(values.size());
    for(std::size_t index = 0; index < values.size(); ++index){
        result[index] = static_cast<std::int64_t>(values[index].value());
    }
    return result;
}

inline std::vector<ModularValue> multiply(
    const std::vector<ModularValue>& left,
    std::size_t left_rows,
    std::size_t inner_size,
    const std::vector<ModularValue>& right,
    std::size_t right_columns
){
    return fast_matrix_multiply(
        left, left_rows, inner_size, right, right_columns
    );
}

}  // namespace fast_matrix_multiply_mod_internal

inline std::vector<std::int64_t> fast_matrix_multiply_mod(
    const std::vector<std::int64_t>& left,
    std::size_t left_rows,
    std::size_t inner_size,
    const std::vector<std::int64_t>& right,
    std::size_t right_columns,
    std::int64_t modulus = 1'000'000'007
){
    using namespace fast_matrix_multiply_mod_internal;
    if(modulus <= 0)[[unlikely]]{
        throw std::invalid_argument(
            "fast_matrix_multiply_mod requires a positive modulus"
        );
    }
    const std::size_t left_area = checked_area(left_rows, inner_size);
    const std::size_t right_area = checked_area(inner_size, right_columns);
    (void)checked_area(left_rows, right_columns);
    if(left.size() != left_area || right.size() != right_area)[[unlikely]]{
        throw std::invalid_argument(
            "fast_matrix_multiply_mod requires matching row-major shapes"
        );
    }
    const std::uint64_t unsigned_modulus =
        static_cast<std::uint64_t>(modulus);
    return decode(multiply(
        encode(left, unsigned_modulus),
        left_rows,
        inner_size,
        encode(right, unsigned_modulus),
        right_columns
    ));
}

}  // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_MATRIX_MULTIPLY_MOD_HPP_INCLUDED
