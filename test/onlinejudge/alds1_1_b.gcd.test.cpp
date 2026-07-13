#define PROBLEM "https://onlinejudge.u-aizu.ac.jp/problems/ALDS1_1_B"

#include <iostream>

#include "../../src/algorithm/math/basic_number_theory.hpp"

int main(){
    long long a, b;
    std::cin >> a >> b;
    std::cout << math::gcd(a, b) << '\n';
}
