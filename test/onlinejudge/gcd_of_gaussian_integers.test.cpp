// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/gcd_of_gaussian_integers

#include <iostream>

#include "../../src/algorithm/math/number_theory/gaussian_integer.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        math::GaussianInteger left, right;
        std::cin >> left.real >> left.imag >> right.real >> right.imag;
        const auto divisor = math::gaussian_gcd(left, right);
        std::cout << divisor.real << ' ' << divisor.imag << '\n';
    }
}
