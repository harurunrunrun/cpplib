// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/multipoint_evaluation

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/multipoint_evaluation.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr int MOD = 998244353;
    using mint = Modint<MOD>;
    using polynomial = math::Polynomial<MOD>;

    std::size_t coefficient_count, point_count;
    std::cin >> coefficient_count >> point_count;
    polynomial coefficients(coefficient_count);
    std::vector<mint> points(point_count);
    for(mint& value: coefficients) std::cin >> value;
    for(mint& value: points) std::cin >> value;

    const std::vector<mint> values = math::multipoint_evaluation<MOD>(
        std::move(coefficients), points
    );
    for(std::size_t index = 0; index < values.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << values[index];
    }
    std::cout << '\n';
}
