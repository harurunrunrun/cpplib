// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/kth_term_of_linearly_recurrent_sequence

#include <cstdint>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/linear_recurrence.hpp"
#include "../../src/structure/modint/modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int degree;
    std::uint64_t index;
    std::cin >> degree >> index;
    std::vector<Modint998244353> initial(static_cast<std::size_t>(degree));
    std::vector<Modint998244353> coefficient(static_cast<std::size_t>(degree));
    for(auto& value: initial) std::cin >> value;
    for(auto& value: coefficient) std::cin >> value;
    std::cout << bostan_mori(initial, coefficient, index) << '\n';
}
