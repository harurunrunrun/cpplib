#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_ROOTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_ROOTS_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

#include "polynomial_factorization_finite_field.hpp"

namespace math{

template<int MOD>
std::vector<Modint<MOD>> polynomial_roots(Polynomial<MOD> polynomial){
    static_assert(polynomial_factorization_finite_field_internal::is_prime_modulus(MOD));
    polynomial_internal::trim(polynomial);
    if(polynomial.empty()) throw std::invalid_argument("zero polynomial has every field element as root");
    if(polynomial.size()==1) return {};
    using namespace polynomial_factorization_finite_field_internal;
    polynomial=make_monic<MOD>(std::move(polynomial));
    auto frobenius=power_mod<MOD>({Modint<MOD>(0),Modint<MOD>(1)},MOD,polynomial);
    if(frobenius.size()<2) frobenius.resize(2);
    frobenius[1]-=Modint<MOD>(1);
    auto linear_product=polynomial_gcd_half<MOD>(polynomial,frobenius);
    if(linear_product.size()<=1) return {};
    DeterministicRandom random;
    std::vector<Polynomial<MOD>> factors;
    equal_degree_factorization<MOD>(std::move(linear_product),1,random,factors);
    std::vector<Modint<MOD>> roots;
    for(const auto& factor:factors) roots.push_back(-factor[0]);
    std::sort(roots.begin(),roots.end(),[](auto x,auto y){return x.val()<y.val();});
    return roots;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_ROOTS_HPP_INCLUDED
