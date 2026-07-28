#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_STIRLING_NUMBERS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_STIRLING_NUMBERS_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "../polynomial/polynomial_taylor_shift.hpp"

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

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_STIRLING_NUMBERS_HPP_INCLUDED
