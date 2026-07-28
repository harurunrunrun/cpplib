// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/polynomial_composite_set_power_series

#include <iostream>
#include <vector>

#include "../../src/algorithm/math/polynomial/set_power_series_composition.hpp"
#include "../../src/structure/modint/modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using Mint = Modint<998244353>;
    int polynomial_size;
    int bit_count;
    std::cin >> polynomial_size >> bit_count;
    std::vector<Mint> polynomial(
        static_cast<std::size_t>(polynomial_size)
    );
    std::vector<Mint> series(std::size_t{1} << bit_count);
    for(Mint& value: polynomial) std::cin >> value;
    for(Mint& value: series) std::cin >> value;
    const auto answer =
        polynomial_composite_set_power_series(polynomial, series);
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
