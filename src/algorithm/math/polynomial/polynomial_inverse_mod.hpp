#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_INVERSE_MOD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_INVERSE_MOD_HPP_INCLUDED

#include <optional>
#include <stdexcept>
#include <utility>

#include "polynomial_half_gcd.hpp"

namespace math{

template<int MOD>
std::optional<Polynomial<MOD>> polynomial_inverse_mod(
    Polynomial<MOD> polynomial, Polynomial<MOD> modulus
){
    polynomial_internal::trim(modulus);
    if(modulus.empty()) throw std::invalid_argument("zero polynomial modulus");
    if(modulus.size()==1) return Polynomial<MOD>{};
    polynomial=polynomial_remainder<MOD>(std::move(polynomial),modulus);
    const auto extended=polynomial_extended_gcd_half<MOD>(polynomial,modulus);
    if(extended.gcd.size()!=1) return std::nullopt;
    return polynomial_remainder<MOD>(extended.left_coefficient,modulus);
}

// One solution of f*h=right mod modulus. Non-coprime f and modulus are allowed.
// The returned representative has degree less than deg(modulus/gcd(f,modulus)).
template<int MOD>
std::optional<Polynomial<MOD>> polynomial_solve_congruence(
    Polynomial<MOD> polynomial,Polynomial<MOD> right,Polynomial<MOD> modulus
){
    polynomial_internal::trim(modulus);
    if(modulus.empty()) throw std::invalid_argument("zero polynomial modulus");
    if(modulus.size()==1) return Polynomial<MOD>{};
    polynomial=polynomial_remainder<MOD>(std::move(polynomial),modulus);
    const auto extended=polynomial_extended_gcd_half<MOD>(polynomial,modulus);
    const auto division=polynomial_divmod<MOD>(std::move(right),extended.gcd);
    if(!division.remainder.empty()) return std::nullopt;
    const auto reduced_modulus=polynomial_divmod<MOD>(modulus,extended.gcd).quotient;
    return polynomial_remainder<MOD>(
        polynomial_multiply<MOD>(extended.left_coefficient,division.quotient),reduced_modulus);
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_INVERSE_MOD_HPP_INCLUDED
