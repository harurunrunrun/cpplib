#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_NEWTON_BASIS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_NEWTON_BASIS_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "multipoint_evaluation.hpp"

namespace math{
namespace polynomial_newton_internal{

template<int MOD>
void convert(const polynomial_internal::ProductTree<MOD>& tree,
    std::size_t node,std::size_t left,std::size_t right,
    Polynomial<MOD> polynomial,Polynomial<MOD>& result
){
    if(left>=result.size()) return;
    if(right-left==1){
        result[left]=polynomial.empty()?Modint<MOD>(0):polynomial[0];
        return;
    }
    auto division=polynomial_divmod<MOD>(std::move(polynomial),tree[node*2]);
    const auto middle=(left+right)/2;
    convert<MOD>(tree,node*2,left,middle,std::move(division.remainder),result);
    convert<MOD>(tree,node*2+1,middle,right,std::move(division.quotient),result);
}

template<int MOD>
Polynomial<MOD> restore(const polynomial_internal::ProductTree<MOD>& tree,
    std::size_t node,std::size_t left,std::size_t right,const Polynomial<MOD>& values
){
    if(left>=values.size()) return {};
    if(right-left==1) return {values[left]};
    const auto middle=(left+right)/2;
    auto low=restore<MOD>(tree,node*2,left,middle,values);
    auto high=restore<MOD>(tree,node*2+1,middle,right,values);
    return polynomial_add<MOD>(low,polynomial_multiply<MOD>(tree[node*2],high));
}

} // namespace polynomial_newton_internal

// f(x)=sum_i result[i]*product_{j<i}(x-points[j]); repeated points allowed.
template<int MOD>
Polynomial<MOD> polynomial_to_newton_basis(
    Polynomial<MOD> polynomial,const Polynomial<MOD>& points
){
    polynomial_internal::trim(polynomial);
    if(polynomial.size()>points.size()) throw std::invalid_argument("Newton basis is too short");
    Polynomial<MOD> result(points.size());
    if(points.empty()) return result;
    const polynomial_internal::ProductTree<MOD> tree(points);
    polynomial_newton_internal::convert<MOD>(tree,1,0,tree.leaf_count(),std::move(polynomial),result);
    return result;
}

template<int MOD>
Polynomial<MOD> polynomial_from_newton_basis(
    const Polynomial<MOD>& values,const Polynomial<MOD>& points
){
    if(values.size()!=points.size()) throw std::invalid_argument("Newton basis size mismatch");
    if(values.empty()) return {};
    const polynomial_internal::ProductTree<MOD> tree(points);
    auto result=polynomial_newton_internal::restore<MOD>(tree,1,0,tree.leaf_count(),values);
    result.resize(values.size());
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_NEWTON_BASIS_HPP_INCLUDED
