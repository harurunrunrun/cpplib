// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/prefix_sum_of_polynomial
#include <iostream>
#include "../../src/algorithm/math/polynomial/polynomial_discrete_sum.hpp"
namespace{
using summation=math::PolynomialDiscreteSum<Modint998244353,524287>;
summation table;
summation::CoefficientArray coefficients;
}
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::cin >> n;
    for(int i=0;i<n;++i) std::cin >> coefficients[i];
    const auto result=table.discrete_antiderivative(coefficients,n);
    for(int i=0;i<=n;++i) std::cout << result[i] << (i==n?'\n':' ');
}
