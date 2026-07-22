// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/counting_squarefrees

#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/number_theory/counting_squarefrees.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::uint64_t n;
    std::cin >> n;
    std::cout << math::count_squarefree_numbers(n) << '\n';
}
