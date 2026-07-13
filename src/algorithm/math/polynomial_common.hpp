#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include "../fft/convolution.hpp"
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

namespace math{

template<int MOD>
using Polynomial = std::vector<Modint<MOD>>;

namespace polynomial_internal{

template<int MOD>
void trim(Polynomial<MOD>& polynomial){
    while(!polynomial.empty() && polynomial.back() == Modint<MOD>(0)){
        polynomial.pop_back();
    }
}

template<int MOD>
Polynomial<MOD> normalized(Polynomial<MOD> polynomial){
    trim(polynomial);
    return polynomial;
}

template<int MOD>
Polynomial<MOD> naive_multiply(
    const Polynomial<MOD>& left,
    const Polynomial<MOD>& right
){
    if(left.empty() || right.empty()) return {};
    Polynomial<MOD> result(left.size() + right.size() - 1);
    for(std::size_t left_index = 0; left_index < left.size(); ++left_index){
        for(std::size_t right_index = 0; right_index < right.size(); ++right_index){
            result[left_index + right_index] += left[left_index] * right[right_index];
        }
    }
    trim(result);
    return result;
}

} // namespace polynomial_internal

template<int MOD>
Polynomial<MOD> polynomial_multiply(
    const Polynomial<MOD>& left,
    const Polynomial<MOD>& right
){
    if(left.empty() || right.empty()) return {};
    constexpr std::size_t naive_work_limit = 4096;
    if(left.size() <= naive_work_limit / right.size()){
        return polynomial_internal::naive_multiply<MOD>(left, right);
    }
    Polynomial<MOD> result = convolution_ntt<MOD>(left, right);
    polynomial_internal::trim(result);
    return result;
}

template<int MOD>
Polynomial<MOD> polynomial_add(
    const Polynomial<MOD>& left,
    const Polynomial<MOD>& right
){
    Polynomial<MOD> result(std::max(left.size(), right.size()));
    for(std::size_t index = 0; index < left.size(); ++index) result[index] += left[index];
    for(std::size_t index = 0; index < right.size(); ++index) result[index] += right[index];
    polynomial_internal::trim(result);
    return result;
}

template<int MOD>
Polynomial<MOD> polynomial_derivative(const Polynomial<MOD>& polynomial){
    if(polynomial.size() <= 1) return {};
    Polynomial<MOD> result(polynomial.size() - 1);
    for(std::size_t index = 1; index < polynomial.size(); ++index){
        result[index - 1] = polynomial[index] *
            Modint<MOD>(static_cast<long long>(index));
    }
    polynomial_internal::trim(result);
    return result;
}

} // namespace math
