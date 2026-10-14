// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/factorization_of_polynomials
#include <cstdint>
#include <iostream>
#include <vector>
#include "../../src/algorithm/math/polynomial/polynomial_factorization_runtime_prime.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::uint32_t prime; std::cin >> n >> prime;
    std::vector<std::uint32_t> polynomial(n+1);
    for(auto& x:polynomial) std::cin >> x;
    const auto result=math::RuntimePrimePolynomialFactorizer(prime)(polynomial);
    std::cout << result.factors.size() << '\n';
    for(const auto& factor:result.factors){
        std::cout << factor.multiplicity << ' ' << factor.polynomial.size()-1;
        for(auto x:factor.polynomial) std::cout << ' ' << x;
        std::cout << '\n';
    }
}
