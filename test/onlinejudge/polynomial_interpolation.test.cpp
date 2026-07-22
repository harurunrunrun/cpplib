// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/polynomial_interpolation

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/polynomial/polynomial_interpolation.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr int MOD = 998244353;
    using mint = Modint<MOD>;
    using polynomial = math::Polynomial<MOD>;

    std::size_t size;
    std::cin >> size;
    std::vector<mint> points(size), values(size);
    for(mint& point: points) std::cin >> point;
    for(mint& value: values) std::cin >> value;

    polynomial coefficients = math::polynomial_interpolation<MOD>(points, values);
    coefficients.resize(size);
    for(std::size_t index = 0; index < size; ++index){
        if(index != 0) std::cout << ' ';
        std::cout << coefficients[index];
    }
    std::cout << '\n';
}
