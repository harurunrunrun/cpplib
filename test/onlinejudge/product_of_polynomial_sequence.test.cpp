// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/product_of_polynomial_sequence

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/polynomial/polynomial_product.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr int MOD = 998244353;
    using mint = Modint<MOD>;
    using polynomial = math::Polynomial<MOD>;

    std::size_t count;
    std::cin >> count;
    std::vector<polynomial> polynomials(count);
    for(polynomial& factor: polynomials){
        std::size_t degree;
        std::cin >> degree;
        factor.resize(degree + 1);
        for(mint& value: factor) std::cin >> value;
    }

    const polynomial result =
        math::product_of_polynomial_sequence<MOD>(polynomials);
    for(std::size_t index = 0; index < result.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << result[index];
    }
    std::cout << '\n';
}
