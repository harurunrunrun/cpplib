#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_TAYLOR_SHIFT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_TAYLOR_SHIFT_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "polynomial_common.hpp"

namespace math{

template<int MOD>
Polynomial<MOD> polynomial_taylor_shift(
    const Polynomial<MOD>& polynomial,
    Modint<MOD> shift
){
    static_assert(MOD >= 2);
    const std::size_t size = polynomial.size();
    if(size == 0) return {};
    if(size >= static_cast<std::size_t>(MOD))[[unlikely]]{
        throw std::length_error(
            "polynomial_taylor_shift requires polynomial.size() < MOD"
        );
    }

    std::vector<Modint<MOD>> factorial(size);
    std::vector<Modint<MOD>> inverse_factorial(size);
    factorial[0] = Modint<MOD>(1);
    for(std::size_t index = 1; index < size; ++index){
        factorial[index] = factorial[index - 1] *
            Modint<MOD>(static_cast<long long>(index));
    }
    inverse_factorial[size - 1] = factorial[size - 1].inv();
    for(std::size_t index = size - 1; index > 0; --index){
        inverse_factorial[index - 1] = inverse_factorial[index] *
            Modint<MOD>(static_cast<long long>(index));
    }

    Polynomial<MOD> reversed(size);
    Polynomial<MOD> exponential(size);
    Modint<MOD> power = 1;
    for(std::size_t index = 0; index < size; ++index){
        reversed[size - 1 - index] = polynomial[index] * factorial[index];
        exponential[index] = power * inverse_factorial[index];
        power *= shift;
    }
    const Polynomial<MOD> product = polynomial_multiply<MOD>(
        reversed, exponential
    );
    Polynomial<MOD> result(size);
    for(std::size_t index = 0; index < size; ++index){
        result[index] = product[size - 1 - index] * inverse_factorial[index];
    }
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_TAYLOR_SHIFT_HPP_INCLUDED
