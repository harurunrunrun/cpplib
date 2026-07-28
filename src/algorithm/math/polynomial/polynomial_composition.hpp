#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_COMPOSITION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_COMPOSITION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <vector>

#include "polynomial_common.hpp"

namespace math{
namespace polynomial_composition_internal{

template<int MOD>
Polynomial<MOD> multiply_prefix(
    const Polynomial<MOD>& left,
    const Polynomial<MOD>& right,
    const std::size_t size
){
    if(size == 0 || left.empty() || right.empty()) return {};
    Polynomial<MOD> result = polynomial_multiply<MOD>(left, right);
    if(result.size() > size) result.resize(size);
    return result;
}

inline std::size_t square_root_ceiling(const std::size_t value){
    if(value <= 1) return value;
    std::size_t result = 1;
    while(result < value / result ||
          (result == value / result && value % result != 0)){
        ++result;
    }
    return result;
}

}  // namespace polynomial_composition_internal

template<int MOD>
Polynomial<MOD> polynomial_compose(
    const Polynomial<MOD>& outer,
    const Polynomial<MOD>& inner,
    const std::size_t size
){
    if(size == 0) return {};
    const std::size_t coefficient_count = std::min(outer.size(), size);
    if(coefficient_count == 0) return Polynomial<MOD>(size);

    const std::size_t block_size =
        polynomial_composition_internal::square_root_ceiling(
            coefficient_count
        );
    std::vector<Polynomial<MOD>> powers(block_size + 1);
    powers[0] = {Modint<MOD>(1)};
    Polynomial<MOD> truncated_inner(
        inner.begin(),
        inner.begin() + static_cast<std::ptrdiff_t>(
            std::min(inner.size(), size)
        )
    );
    polynomial_internal::trim(truncated_inner);
    for(std::size_t exponent = 1; exponent <= block_size; ++exponent){
        powers[exponent] =
            polynomial_composition_internal::multiply_prefix<MOD>(
                powers[exponent - 1], truncated_inner, size
            );
    }
    const Polynomial<MOD>& giant_step = powers[block_size];
    const std::size_t block_count =
        coefficient_count / block_size
        + static_cast<std::size_t>(coefficient_count % block_size != 0);

    Polynomial<MOD> result;
    for(std::size_t block = block_count; block-- > 0;){
        result = polynomial_composition_internal::multiply_prefix<MOD>(
            result, giant_step, size
        );
        const std::size_t first = block * block_size;
        const std::size_t last = std::min(
            coefficient_count, first + block_size
        );
        if(result.size() < size) result.resize(size);
        for(std::size_t index = first; index < last; ++index){
            const Modint<MOD> coefficient = outer[index];
            if(coefficient == Modint<MOD>(0)) continue;
            const Polynomial<MOD>& power = powers[index - first];
            for(std::size_t degree = 0; degree < power.size(); ++degree){
                result[degree] += coefficient * power[degree];
            }
        }
        polynomial_internal::trim(result);
    }
    result.resize(size);
    return result;
}

}  // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_COMPOSITION_HPP_INCLUDED
