#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_MIDDLE_PRODUCT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_MIDDLE_PRODUCT_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>

#include "polynomial_common.hpp"

namespace math{

template<int MOD>
Polynomial<MOD> polynomial_middle_product(
    const Polynomial<MOD>& values,
    const Polynomial<MOD>& kernel
){
    if(kernel.empty())[[unlikely]]{
        throw std::invalid_argument(
            "polynomial middle product requires a nonempty kernel"
        );
    }
    if(values.size() < kernel.size()) return {};
    Polynomial<MOD> reversed_kernel(kernel.rbegin(), kernel.rend());
    Polynomial<MOD> product = polynomial_multiply<MOD>(
        values, reversed_kernel
    );
    const std::size_t result_size = values.size() - kernel.size() + 1;
    Polynomial<MOD> result(result_size);
    const std::size_t offset = kernel.size() - 1;
    for(std::size_t index = 0; index < result_size; ++index){
        if(offset + index < product.size()){
            result[index] = product[offset + index];
        }
    }
    return result;
}

}  // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_MIDDLE_PRODUCT_HPP_INCLUDED
