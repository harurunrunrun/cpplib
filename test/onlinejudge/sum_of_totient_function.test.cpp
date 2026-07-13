// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sum_of_totient_function

#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/sum_of_totient_function.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::uint64_t n;
    std::cin >> n;
    std::cout << math::sum_of_totient_function(n) << '\n';
}
