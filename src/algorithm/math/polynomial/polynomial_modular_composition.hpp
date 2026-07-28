#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_MODULAR_COMPOSITION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_MODULAR_COMPOSITION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "polynomial_composition.hpp"
#include "polynomial_division.hpp"

namespace math{
namespace polynomial_modular_composition_internal{

template<int MOD>
Polynomial<MOD> multiply_mod(
    const Polynomial<MOD>& left,
    const Polynomial<MOD>& right,
    const Polynomial<MOD>& modulus
){
    return polynomial_remainder<MOD>(
        polynomial_multiply<MOD>(left, right), modulus
    );
}

}  // namespace polynomial_modular_composition_internal

template<int MOD>
Polynomial<MOD> polynomial_compose_mod(
    const Polynomial<MOD>& outer,
    const Polynomial<MOD>& inner,
    Polynomial<MOD> modulus
){
    polynomial_internal::trim(modulus);
    if(modulus.size() <= 1)[[unlikely]]{
        throw std::invalid_argument(
            "polynomial modular composition requires positive-degree modulus"
        );
    }
    if(outer.empty()) return {};
    const Polynomial<MOD> reduced_inner = polynomial_remainder<MOD>(
        inner, modulus
    );
    const std::size_t block_size =
        polynomial_composition_internal::square_root_ceiling(outer.size());
    std::vector<Polynomial<MOD>> powers(block_size + 1);
    powers[0] = {Modint<MOD>(1)};
    for(std::size_t exponent = 1; exponent <= block_size; ++exponent){
        powers[exponent] =
            polynomial_modular_composition_internal::multiply_mod<MOD>(
                powers[exponent - 1], reduced_inner, modulus
            );
    }
    const Polynomial<MOD>& giant_step = powers[block_size];
    const std::size_t block_count = outer.size() / block_size
        + static_cast<std::size_t>(outer.size() % block_size != 0);
    Polynomial<MOD> result;
    for(std::size_t block = block_count; block-- > 0;){
        result = polynomial_modular_composition_internal::multiply_mod<MOD>(
            result, giant_step, modulus
        );
        const std::size_t first = block * block_size;
        const std::size_t last = std::min(
            outer.size(), first + block_size
        );
        if(result.size() < modulus.size() - 1){
            result.resize(modulus.size() - 1);
        }
        for(std::size_t index = first; index < last; ++index){
            const Modint<MOD> coefficient = outer[index];
            if(coefficient == Modint<MOD>(0)) continue;
            for(std::size_t degree = 0;
                degree < powers[index - first].size(); ++degree){
                result[degree] += coefficient
                    * powers[index - first][degree];
            }
        }
        polynomial_internal::trim(result);
    }
    return result;
}

}  // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_MODULAR_COMPOSITION_HPP_INCLUDED
