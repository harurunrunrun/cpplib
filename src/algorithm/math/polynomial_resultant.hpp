#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_RESULTANT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_RESULTANT_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "polynomial_division.hpp"

template<class T, std::size_t CAPACITY>
struct PolynomialRemainderResult{
    std::array<T, CAPACITY> coefficients{};
    std::size_t coefficient_count = 0;
};

namespace polynomial_resultant_internal{

template<class T>
struct static_modint_traits{
    static constexpr bool value = false;
};

template<int MOD>
struct static_modint_traits<Modint<MOD>>{
    static constexpr bool value = true;
    static constexpr int modulus = MOD;
};

constexpr bool is_prime(const int value){
    if(value < 2) return false;
    if(value % 2 == 0) return value == 2;
    for(int divisor = 3;
        static_cast<long long>(divisor) * divisor <= value;
        divisor += 2
    ){
        if(value % divisor == 0) return false;
    }
    return true;
}

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

template<int MOD>
bool fast_transform_is_available(const std::size_t coefficient_count){
    if constexpr(!is_prime(MOD)) return false;
    if(coefficient_count <= 1) return true;
    if(coefficient_count >
       (std::numeric_limits<std::size_t>::max() / 2)) return false;
    const std::size_t required = coefficient_count * 2 - 1;
    std::size_t transform_size = 1;
    while(transform_size < required){
        if(transform_size >
           (std::numeric_limits<std::size_t>::max() / 2)) return false;
        transform_size *= 2;
    }
    return transform_size <= static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ) && static_cast<std::size_t>(MOD - 1) % transform_size == 0;
}

template<int MOD>
struct FastPolynomialMatrix{
    math::Polynomial<MOD> top_left;
    math::Polynomial<MOD> top_right;
    math::Polynomial<MOD> bottom_left;
    math::Polynomial<MOD> bottom_right;
};

template<int MOD>
math::Polynomial<MOD> fast_negate(math::Polynomial<MOD> value){
    for(Modint<MOD>& coefficient: value) coefficient = -coefficient;
    math::polynomial_internal::trim(value);
    return value;
}

template<int MOD>
math::Polynomial<MOD> fast_high_part(
    const math::Polynomial<MOD>& value,
    const std::size_t skipped_coefficients
){
    if(skipped_coefficients >= value.size()) return {};
    return math::Polynomial<MOD>(
        value.begin() + static_cast<std::ptrdiff_t>(skipped_coefficients),
        value.end()
    );
}

template<int MOD>
FastPolynomialMatrix<MOD> fast_identity_matrix(){
    return {{Modint<MOD>(1)}, {}, {}, {Modint<MOD>(1)}};
}

template<int MOD>
FastPolynomialMatrix<MOD> fast_multiply_matrix(
    const FastPolynomialMatrix<MOD>& left,
    const FastPolynomialMatrix<MOD>& right
){
    return {
        math::polynomial_add<MOD>(
            math::polynomial_multiply<MOD>(
                left.top_left, right.top_left
            ),
            math::polynomial_multiply<MOD>(
                left.top_right, right.bottom_left
            )
        ),
        math::polynomial_add<MOD>(
            math::polynomial_multiply<MOD>(
                left.top_left, right.top_right
            ),
            math::polynomial_multiply<MOD>(
                left.top_right, right.bottom_right
            )
        ),
        math::polynomial_add<MOD>(
            math::polynomial_multiply<MOD>(
                left.bottom_left, right.top_left
            ),
            math::polynomial_multiply<MOD>(
                left.bottom_right, right.bottom_left
            )
        ),
        math::polynomial_add<MOD>(
            math::polynomial_multiply<MOD>(
                left.bottom_left, right.top_right
            ),
            math::polynomial_multiply<MOD>(
                left.bottom_right, right.bottom_right
            )
        )
    };
}

template<int MOD>
std::pair<math::Polynomial<MOD>, math::Polynomial<MOD>>
fast_apply_matrix(
    const FastPolynomialMatrix<MOD>& matrix,
    const math::Polynomial<MOD>& first,
    const math::Polynomial<MOD>& second
){
    return {
        math::polynomial_add<MOD>(
            math::polynomial_multiply<MOD>(matrix.top_left, first),
            math::polynomial_multiply<MOD>(matrix.top_right, second)
        ),
        math::polynomial_add<MOD>(
            math::polynomial_multiply<MOD>(matrix.bottom_left, first),
            math::polynomial_multiply<MOD>(matrix.bottom_right, second)
        )
    };
}

template<int MOD>
struct FastResultantReduction{
    FastPolynomialMatrix<MOD> matrix = fast_identity_matrix<MOD>();
    Modint<MOD> factor = Modint<MOD>(1);
    bool step_count_parity = false;
    bool adjacent_degree_sum_parity = false;
};

template<int MOD>
FastResultantReduction<MOD> fast_lift_reduction(
    FastResultantReduction<MOD> reduction,
    const std::size_t degree_shift
){
    if(
        (degree_shift & 1U) != 0 &&
        (reduction.step_count_parity !=
         reduction.adjacent_degree_sum_parity)
    ){
        reduction.factor = -reduction.factor;
    }
    return reduction;
}

template<int MOD>
FastResultantReduction<MOD> fast_combine_reductions(
    const FastResultantReduction<MOD>& first,
    const FastResultantReduction<MOD>& second
){
    return {
        fast_multiply_matrix<MOD>(second.matrix, first.matrix),
        first.factor * second.factor,
        first.step_count_parity != second.step_count_parity,
        first.adjacent_degree_sum_parity !=
            second.adjacent_degree_sum_parity
    };
}

template<int MOD>
FastResultantReduction<MOD> fast_euclidean_reduction(
    const math::Polynomial<MOD>& first,
    const math::Polynomial<MOD>& second,
    math::Polynomial<MOD> quotient,
    const math::Polynomial<MOD>& remainder
){
    const std::size_t first_degree = first.size() - 1;
    const std::size_t second_degree = second.size() - 1;
    const std::size_t remainder_degree = remainder.size() - 1;
    Modint<MOD> factor = power(
        second.back(), first_degree - remainder_degree
    );
    if(
        (first_degree & 1U) != 0 &&
        (second_degree & 1U) != 0
    ){
        factor = -factor;
    }
    return {
        {{}, {Modint<MOD>(1)},
         {Modint<MOD>(1)}, fast_negate<MOD>(std::move(quotient))},
        factor,
        true,
        ((first_degree + second_degree) & 1U) != 0
    };
}

template<int MOD>
FastResultantReduction<MOD> fast_half_resultant_reduction(
    const math::Polynomial<MOD>& first,
    const math::Polynomial<MOD>& second
){
    if(first.empty() || first.size() <= second.size()) return {};
    const std::size_t target_degree = first.size() / 2;
    if(second.empty() || second.size() - 1 < target_degree){
        return {};
    }

    FastResultantReduction<MOD> first_reduction =
        fast_half_resultant_reduction<MOD>(
            fast_high_part<MOD>(first, target_degree),
            fast_high_part<MOD>(second, target_degree)
        );
    first_reduction = fast_lift_reduction<MOD>(
        std::move(first_reduction), target_degree
    );
    auto [reduced_first, reduced_second] = fast_apply_matrix<MOD>(
        first_reduction.matrix, first, second
    );
    if(
        reduced_second.empty() ||
        reduced_second.size() - 1 < target_degree
    ){
        return first_reduction;
    }

    math::PolynomialDivModResult<MOD> division =
        math::polynomial_divmod<MOD>(reduced_first, reduced_second);
    if(division.remainder.empty()){
        return first_reduction;
    }
    const FastResultantReduction<MOD> euclidean =
        fast_euclidean_reduction<MOD>(
            reduced_first,
            reduced_second,
            std::move(division.quotient),
            division.remainder
        );
    FastResultantReduction<MOD> prefix =
        fast_combine_reductions<MOD>(first_reduction, euclidean);

    const std::size_t shift = 2 * target_degree -
        (reduced_second.size() - 1);
    FastResultantReduction<MOD> second_reduction =
        fast_half_resultant_reduction<MOD>(
            fast_high_part<MOD>(reduced_second, shift),
            fast_high_part<MOD>(division.remainder, shift)
        );
    second_reduction = fast_lift_reduction<MOD>(
        std::move(second_reduction), shift
    );
    return fast_combine_reductions<MOD>(prefix, second_reduction);
}


template<int MOD, std::size_t CAPACITY>
Modint<MOD> fast_resultant(
    const std::array<Modint<MOD>, CAPACITY>& left,
    const std::size_t left_count,
    const std::array<Modint<MOD>, CAPACITY>& right,
    const std::size_t right_count
){
    math::Polynomial<MOD> first(
        left.begin(),
        left.begin() + static_cast<std::ptrdiff_t>(left_count)
    );
    math::Polynomial<MOD> second(
        right.begin(),
        right.begin() + static_cast<std::ptrdiff_t>(right_count)
    );
    if(first.empty() || second.empty()) return Modint<MOD>(0);

    Modint<MOD> result = Modint<MOD>(1);
    if(first.size() < second.size()){
        if(
            ((first.size() - 1) & 1U) != 0 &&
            ((second.size() - 1) & 1U) != 0
        ){
            result = -result;
        }
        std::swap(first, second);
    }

    while(second.size() > 1){
        const FastResultantReduction<MOD> reduction =
            fast_half_resultant_reduction<MOD>(first, second);
        auto reduced = fast_apply_matrix<MOD>(
            reduction.matrix, first, second
        );
        first = std::move(reduced.first);
        second = std::move(reduced.second);
        result *= reduction.factor;
        if(second.size() <= 1) break;

        math::PolynomialDivModResult<MOD> division =
            math::polynomial_divmod<MOD>(first, second);
        if(division.remainder.empty()) return Modint<MOD>(0);
        const FastResultantReduction<MOD> euclidean =
            fast_euclidean_reduction<MOD>(
                first,
                second,
                std::move(division.quotient),
                division.remainder
            );
        result *= euclidean.factor;
        first = std::move(second);
        second = std::move(division.remainder);
    }

    return result * power(second.front(), first.size() - 1);
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

    if constexpr(
        polynomial_resultant_internal::static_modint_traits<T>::value
    ){
        constexpr int modulus =
            polynomial_resultant_internal::static_modint_traits<T>::modulus;
        const std::size_t trimmed_left =
            polynomial_resultant_internal::trim_count(left, left_count);
        const std::size_t trimmed_right =
            polynomial_resultant_internal::trim_count(right, right_count);
        const std::size_t maximum_count =
            trimmed_left > trimmed_right ? trimmed_left : trimmed_right;
        if(polynomial_resultant_internal::fast_transform_is_available<modulus>(
            maximum_count
        )){
            return polynomial_resultant_internal::fast_resultant<modulus>(
                left, trimmed_left, right, trimmed_right
            );
        }
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

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_RESULTANT_HPP_INCLUDED
