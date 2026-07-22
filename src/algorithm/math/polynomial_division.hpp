#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_DIVISION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_DIVISION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>

#include "../fft/formal_power_series.hpp"
#include "polynomial_common.hpp"

namespace math{

template<int MOD>
struct PolynomialDivModResult{
    Polynomial<MOD> quotient;
    Polynomial<MOD> remainder;
};

namespace polynomial_internal{

template<int MOD>
PolynomialDivModResult<MOD> naive_divmod(
    const Polynomial<MOD>& dividend,
    const Polynomial<MOD>& divisor
){
    Polynomial<MOD> work = dividend;
    const std::size_t quotient_size = dividend.size() - divisor.size() + 1;
    Polynomial<MOD> quotient(quotient_size);
    const Modint<MOD> inverse_leading = divisor.back().inv();
    for(std::size_t current = dividend.size(); current >= divisor.size(); --current){
        const std::size_t shift = current - divisor.size();
        const Modint<MOD> factor = work[current - 1] * inverse_leading;
        quotient[shift] = factor;
        for(std::size_t degree = 0; degree < divisor.size(); ++degree){
            work[shift + degree] -= factor * divisor[degree];
        }
    }
    work.resize(divisor.size() - 1);
    trim(quotient);
    trim(work);
    return {std::move(quotient), std::move(work)};
}

} // namespace polynomial_internal

template<int MOD>
PolynomialDivModResult<MOD> polynomial_divmod(
    Polynomial<MOD> dividend,
    Polynomial<MOD> divisor
){
    polynomial_internal::trim(dividend);
    polynomial_internal::trim(divisor);
    if(divisor.empty())[[unlikely]]{
        throw std::invalid_argument("polynomial division by zero");
    }
    if(dividend.size() < divisor.size()){
        return {{}, std::move(dividend)};
    }
    if(divisor.size() == 1){
        const Modint<MOD> inverse = divisor[0].inv();
        for(Modint<MOD>& coefficient: dividend) coefficient *= inverse;
        polynomial_internal::trim(dividend);
        return {std::move(dividend), {}};
    }

    constexpr std::size_t naive_work_limit = 4096;
    if(dividend.size() <= naive_work_limit / divisor.size()){
        return polynomial_internal::naive_divmod<MOD>(dividend, divisor);
    }

    const std::size_t quotient_size = dividend.size() - divisor.size() + 1;
    Polynomial<MOD> reversed_dividend(quotient_size);
    for(std::size_t index = 0; index < quotient_size; ++index){
        reversed_dividend[index] = dividend[dividend.size() - 1 - index];
    }
    Polynomial<MOD> reversed_divisor(
        divisor.rbegin(),
        divisor.rbegin() + static_cast<std::ptrdiff_t>(
            std::min(divisor.size(), quotient_size)
        )
    );
    const Polynomial<MOD> inverse = fps_inv<MOD>(reversed_divisor, quotient_size);
    Polynomial<MOD> reversed_quotient = polynomial_multiply<MOD>(
        reversed_dividend, inverse
    );
    reversed_quotient.resize(quotient_size);
    std::reverse(reversed_quotient.begin(), reversed_quotient.end());
    Polynomial<MOD> quotient = std::move(reversed_quotient);
    polynomial_internal::trim(quotient);

    const Polynomial<MOD> product = polynomial_multiply<MOD>(quotient, divisor);
    Polynomial<MOD> remainder(divisor.size() - 1);
    for(std::size_t index = 0; index < remainder.size(); ++index){
        remainder[index] = dividend[index];
        if(index < product.size()) remainder[index] -= product[index];
    }
    polynomial_internal::trim(remainder);
    return {std::move(quotient), std::move(remainder)};
}

template<int MOD>
Polynomial<MOD> polynomial_remainder(
    Polynomial<MOD> dividend,
    Polynomial<MOD> divisor
){
    return polynomial_divmod<MOD>(std::move(dividend), std::move(divisor)).remainder;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_DIVISION_HPP_INCLUDED
