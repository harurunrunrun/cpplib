#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_REVERSION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_REVERSION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>

#include "../../fft/formal_power_series/formal_power_series.hpp"
#include "polynomial_composition.hpp"

namespace math{

template<int MOD>
Polynomial<MOD> polynomial_reversion(
    const Polynomial<MOD>& function,
    const std::size_t size
){
    if(size == 0) return {};
    if(function.size() < 2 ||
       function[0] != Modint<MOD>(0) ||
       function[1] == Modint<MOD>(0))[[unlikely]]{
        throw std::invalid_argument(
            "polynomial reversion requires f(0)=0 and nonzero f'(0)"
        );
    }
    Polynomial<MOD> inverse(size == 1 ? 1 : 2);
    if(size == 1) return inverse;
    inverse[1] = function[1].inv();
    const Polynomial<MOD> derivative = polynomial_derivative<MOD>(function);

    while(inverse.size() < size){
        const std::size_t next_size = std::min(size, inverse.size() * 2);
        Polynomial<MOD> error = polynomial_compose<MOD>(
            function, inverse, next_size
        );
        if(error.size() < next_size) error.resize(next_size);
        error[1] -= Modint<MOD>(1);

        Polynomial<MOD> composed_derivative = polynomial_compose<MOD>(
            derivative, inverse, next_size
        );
        const Polynomial<MOD> reciprocal_derivative = fps_inv<MOD>(
            composed_derivative, next_size
        );
        Polynomial<MOD> correction =
            polynomial_composition_internal::multiply_prefix<MOD>(
                error, reciprocal_derivative, next_size
            );
        correction.resize(next_size);
        inverse.resize(next_size);
        for(std::size_t index = 0; index < next_size; ++index){
            inverse[index] -= correction[index];
        }
        inverse[0] = Modint<MOD>(0);
    }
    return inverse;
}

}  // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_REVERSION_HPP_INCLUDED
