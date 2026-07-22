// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/division_of_polynomials

#include <cstddef>
#include <iostream>
#include <utility>

#include "../../src/algorithm/math/polynomial/polynomial_division.hpp"

namespace{

constexpr int MOD = 998244353;
using mint = Modint<MOD>;
using polynomial = math::Polynomial<MOD>;

void print_coefficients(const polynomial& values){
    for(std::size_t index = 0; index < values.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << values[index];
    }
    std::cout << '\n';
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t dividend_size, divisor_size;
    std::cin >> dividend_size >> divisor_size;
    polynomial dividend(dividend_size), divisor(divisor_size);
    for(mint& value: dividend) std::cin >> value;
    for(mint& value: divisor) std::cin >> value;

    const auto result = math::polynomial_divmod<MOD>(
        std::move(dividend), std::move(divisor)
    );
    std::cout << result.quotient.size() << ' ' << result.remainder.size() << '\n';
    print_coefficients(result.quotient);
    print_coefficients(result.remainder);
}
