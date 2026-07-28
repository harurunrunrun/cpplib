#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_STIRLING_NUMBERS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_STIRLING_NUMBERS_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "../polynomial/polynomial_taylor_shift.hpp"
#include "../../fft/formal_power_series/formal_power_series.hpp"

namespace math{
namespace stirling_internal{

template<int MOD>
Polynomial<MOD> rising_factorial_polynomial(std::size_t count){
    if(count == 0) return {Modint<MOD>(1)};
    if((count & 1U) != 0){
        Polynomial<MOD> result = rising_factorial_polynomial<MOD>(count - 1);
        return polynomial_multiply<MOD>(
            result,
            {
                Modint<MOD>(static_cast<long long>(count - 1)),
                Modint<MOD>(1)
            }
        );
    }
    const std::size_t half = count / 2;
    Polynomial<MOD> first = rising_factorial_polynomial<MOD>(half);
    Polynomial<MOD> second = polynomial_taylor_shift<MOD>(
        first, Modint<MOD>(static_cast<long long>(half))
    );
    return polynomial_multiply<MOD>(first, second);
}

template<int MOD>
void factorial_tables(
    std::size_t maximum,
    std::vector<Modint<MOD>>& factorial,
    std::vector<Modint<MOD>>& inverse_factorial
){
    if(maximum >= static_cast<std::size_t>(MOD))[[unlikely]]{
        throw std::length_error(
            "Stirling numbers require maximum_index < MOD"
        );
    }
    factorial.assign(maximum + 1, Modint<MOD>(1));
    inverse_factorial.assign(maximum + 1, Modint<MOD>(1));
    for(std::size_t index = 1; index <= maximum; ++index){
        factorial[index] = factorial[index - 1] *
            Modint<MOD>(static_cast<long long>(index));
    }
    inverse_factorial[maximum] = factorial[maximum].inv();
    for(std::size_t index = maximum; index > 0; --index){
        inverse_factorial[index - 1] = inverse_factorial[index] *
            Modint<MOD>(static_cast<long long>(index));
    }
}

} // namespace stirling_internal

template<int MOD>
std::vector<Modint<MOD>> signed_stirling_numbers_first_kind(
    std::size_t order
){
    if(order >= static_cast<std::size_t>(MOD))[[unlikely]]{
        throw std::length_error(
            "signed_stirling_numbers_first_kind requires order < MOD"
        );
    }
    Polynomial<MOD> result =
        stirling_internal::rising_factorial_polynomial<MOD>(order);
    result.resize(order + 1);
    for(std::size_t degree = 0; degree <= order; ++degree){
        if(((order - degree) & 1U) != 0) result[degree] = -result[degree];
    }
    return result;
}

template<int MOD>
std::vector<Modint<MOD>> stirling_numbers_second_kind(std::size_t order){
    std::vector<Modint<MOD>> factorial;
    std::vector<Modint<MOD>> inverse_factorial;
    stirling_internal::factorial_tables<MOD>(
        order, factorial, inverse_factorial
    );
    Polynomial<MOD> powers(order + 1);
    Polynomial<MOD> signs(order + 1);
    for(std::size_t index = 0; index <= order; ++index){
        powers[index] =
            Modint<MOD>(static_cast<long long>(index)).pow(
                static_cast<long long>(order)
            ) * inverse_factorial[index];
        signs[index] = ((index & 1U) == 0 ? inverse_factorial[index]
                                          : -inverse_factorial[index]);
    }
    Polynomial<MOD> result = polynomial_multiply<MOD>(powers, signs);
    result.resize(order + 1);
    return result;
}

template<int MOD>
std::vector<Modint<MOD>> signed_stirling_numbers_first_kind_fixed_k(
    std::size_t maximum_order,
    std::size_t fixed_cycles
){
    if(fixed_cycles > maximum_order)[[unlikely]]{
        throw std::invalid_argument(
            "fixed_cycles must not exceed maximum_order"
        );
    }
    std::vector<Modint<MOD>> factorial;
    std::vector<Modint<MOD>> inverse_factorial;
    stirling_internal::factorial_tables<MOD>(
        maximum_order, factorial, inverse_factorial
    );
    std::vector<Modint<MOD>> logarithm(maximum_order + 1);
    for(std::size_t index = 1; index <= maximum_order; ++index){
        const Modint<MOD> inverse =
            Modint<MOD>(static_cast<long long>(index)).inv();
        logarithm[index] = ((index & 1U) == 0 ? -inverse : inverse);
    }
    std::vector<Modint<MOD>> result = fps_pow<MOD>(
        logarithm,
        static_cast<long long>(fixed_cycles),
        maximum_order + 1
    );
    const Modint<MOD> column_scale = inverse_factorial[fixed_cycles];
    for(std::size_t order = 0; order <= maximum_order; ++order){
        result[order] *= factorial[order] * column_scale;
    }
    return result;
}

template<int MOD>
std::vector<Modint<MOD>> stirling_numbers_second_kind_fixed_k(
    std::size_t maximum_order,
    std::size_t fixed_parts
){
    if(fixed_parts > maximum_order)[[unlikely]]{
        throw std::invalid_argument(
            "fixed_parts must not exceed maximum_order"
        );
    }
    std::vector<Modint<MOD>> factorial;
    std::vector<Modint<MOD>> inverse_factorial;
    stirling_internal::factorial_tables<MOD>(
        maximum_order, factorial, inverse_factorial
    );
    std::vector<Modint<MOD>> exponential_minus_one(maximum_order + 1);
    for(std::size_t index = 1; index <= maximum_order; ++index){
        exponential_minus_one[index] = inverse_factorial[index];
    }
    std::vector<Modint<MOD>> result = fps_pow<MOD>(
        exponential_minus_one,
        static_cast<long long>(fixed_parts),
        maximum_order + 1
    );
    const Modint<MOD> column_scale = inverse_factorial[fixed_parts];
    for(std::size_t order = 0; order <= maximum_order; ++order){
        result[order] *= factorial[order] * column_scale;
    }
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_STIRLING_NUMBERS_HPP_INCLUDED
