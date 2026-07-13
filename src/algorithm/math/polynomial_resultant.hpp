#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>

template<class T, std::size_t CAPACITY>
struct PolynomialRemainderResult{
    std::array<T, CAPACITY> coefficients{};
    std::size_t coefficient_count = 0;
};

namespace polynomial_resultant_internal{

template<class T, std::size_t CAPACITY>
std::size_t trim_count(
    const std::array<T, CAPACITY>& coefficients,
    std::size_t coefficient_count
){
    while(
        coefficient_count > 0 &&
        coefficients[coefficient_count - 1] == T(0)
    ){
        --coefficient_count;
    }
    return coefficient_count;
}

template<class T>
T power(T base, std::size_t exponent){
    T result = T(1);
    while(exponent > 0){
        if((exponent & 1U) != 0) result *= base;
        base *= base;
        exponent >>= 1;
    }
    return result;
}

template<class T, std::size_t CAPACITY>
std::size_t remainder_in_place(
    std::array<T, CAPACITY>& work,
    std::size_t dividend_count,
    const std::array<T, CAPACITY>& divisor,
    std::size_t divisor_count
){
    if(dividend_count < divisor_count) return dividend_count;
    const std::size_t divisor_degree = divisor_count - 1;
    const T inverse_leading = T(1) / divisor[divisor_degree];
    for(std::size_t current = dividend_count;
        current >= divisor_count;
        --current
    ){
        const std::size_t shift = current - divisor_count;
        const T factor = work[current - 1] * inverse_leading;
        work[current - 1] = T(0);
        for(std::size_t degree = 0; degree < divisor_degree; ++degree){
            work[shift + degree] -= factor * divisor[degree];
        }
    }
    return trim_count(work, divisor_degree);
}

} // namespace polynomial_resultant_internal

template<class T, std::size_t CAPACITY>
PolynomialRemainderResult<T, CAPACITY> polynomial_remainder(
    const std::array<T, CAPACITY>& dividend,
    std::size_t dividend_count,
    const std::array<T, CAPACITY>& divisor,
    std::size_t divisor_count
){
    static_assert(CAPACITY > 0);
    if(dividend_count > CAPACITY || divisor_count > CAPACITY)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (polynomial_remainder)."
        );
    }
    dividend_count = polynomial_resultant_internal::trim_count(
        dividend, dividend_count
    );
    divisor_count = polynomial_resultant_internal::trim_count(
        divisor, divisor_count
    );
    if(divisor_count == 0)[[unlikely]]{
        throw std::invalid_argument("polynomial division by zero");
    }

    PolynomialRemainderResult<T, CAPACITY> result;
    result.coefficients = dividend;
    result.coefficient_count =
        polynomial_resultant_internal::remainder_in_place(
            result.coefficients, dividend_count, divisor, divisor_count
        );
    return result;
}

template<class T, std::size_t CAPACITY>
T polynomial_resultant(
    const std::array<T, CAPACITY>& left,
    std::size_t left_count,
    const std::array<T, CAPACITY>& right,
    std::size_t right_count
){
    static_assert(CAPACITY > 0);
    if(left_count > CAPACITY || right_count > CAPACITY)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (polynomial_resultant)."
        );
    }

    std::array<T, CAPACITY> first = left;
    std::array<T, CAPACITY> second = right;
    std::array<T, CAPACITY> remainder{};
    auto* first_pointer = &first;
    auto* second_pointer = &second;
    auto* remainder_pointer = &remainder;
    left_count = polynomial_resultant_internal::trim_count(first, left_count);
    right_count = polynomial_resultant_internal::trim_count(second, right_count);
    if(left_count == 0 || right_count == 0) return T(0);

    T result = T(1);
    std::size_t left_degree = left_count - 1;
    std::size_t right_degree = right_count - 1;
    if(left_degree < right_degree){
        if((left_degree & 1U) != 0 && (right_degree & 1U) != 0){
            result = -result;
        }
        std::swap(first_pointer, second_pointer);
        std::swap(left_count, right_count);
        std::swap(left_degree, right_degree);
    }

    while(right_degree > 0){
        for(std::size_t index = 0; index < left_count; ++index){
            (*remainder_pointer)[index] = (*first_pointer)[index];
        }
        const std::size_t remainder_count =
            polynomial_resultant_internal::remainder_in_place(
                *remainder_pointer,
                left_count,
                *second_pointer,
                right_count
            );
        if(remainder_count == 0) return T(0);
        const std::size_t remainder_degree = remainder_count - 1;

        if((left_degree & 1U) != 0 && (right_degree & 1U) != 0){
            result = -result;
        }
        result *= polynomial_resultant_internal::power(
            (*second_pointer)[right_degree],
            left_degree - remainder_degree
        );

        auto* old_first = first_pointer;
        first_pointer = second_pointer;
        second_pointer = remainder_pointer;
        remainder_pointer = old_first;
        left_count = right_count;
        right_count = remainder_count;
        left_degree = right_degree;
        right_degree = remainder_degree;
    }

    result *= polynomial_resultant_internal::power(
        (*second_pointer)[0], left_degree
    );
    return result;
}

template<class T, std::size_t CAPACITY>
T polynomial_discriminant(
    const std::array<T, CAPACITY>& coefficients,
    std::size_t coefficient_count
){
    static_assert(CAPACITY > 0);
    if(coefficient_count > CAPACITY)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (polynomial_discriminant)."
        );
    }
    coefficient_count = polynomial_resultant_internal::trim_count(
        coefficients, coefficient_count
    );
    if(coefficient_count == 0)[[unlikely]]{
        throw std::invalid_argument("zero polynomial has no discriminant");
    }
    const std::size_t degree = coefficient_count - 1;
    if(degree == 0) return T(1);

    std::array<T, CAPACITY> derivative{};
    for(std::size_t index = 1; index < coefficient_count; ++index){
        derivative[index - 1] = coefficients[index] *
            T(static_cast<long long>(index));
    }
    T result = polynomial_resultant(
        coefficients, coefficient_count, derivative, degree
    ) / coefficients[degree];
    if(((degree * (degree - 1) / 2) & 1U) != 0) result = -result;
    return result;
}
