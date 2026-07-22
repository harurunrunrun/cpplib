#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_PRODUCT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_PRODUCT_HPP_INCLUDED

#include <cstddef>
#include <utility>
#include <vector>

#include "polynomial_common.hpp"

namespace math{

template<int MOD>
Polynomial<MOD> product_of_polynomial_sequence(
    const std::vector<Polynomial<MOD>>& polynomials
){
    if(polynomials.empty()) return {Modint<MOD>(1)};
    std::vector<Polynomial<MOD>> layer;
    layer.reserve(polynomials.size());
    for(Polynomial<MOD> polynomial: polynomials){
        polynomial_internal::trim(polynomial);
        if(polynomial.empty()) return {};
        layer.push_back(std::move(polynomial));
    }

    while(layer.size() > 1){
        std::vector<Polynomial<MOD>> next;
        next.reserve((layer.size() + 1) / 2);
        for(std::size_t index = 0; index + 1 < layer.size(); index += 2){
            next.push_back(polynomial_multiply<MOD>(layer[index], layer[index + 1]));
        }
        if((layer.size() & 1U) != 0) next.push_back(std::move(layer.back()));
        layer = std::move(next);
    }
    return std::move(layer.front());
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_PRODUCT_HPP_INCLUDED
