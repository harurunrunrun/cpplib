// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/characteristic_polynomial

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/linear_algebra/characteristic_polynomial.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<std::vector<FastModint998244353>> matrix(
        static_cast<std::size_t>(size),
        std::vector<FastModint998244353>(static_cast<std::size_t>(size))
    );
    for(auto& row: matrix){
        for(FastModint998244353& value: row) std::cin >> value;
    }
    const auto result = characteristic_polynomial(std::move(matrix));
    for(std::size_t degree = 0; degree < result.size(); ++degree){
        if(degree != 0) std::cout << ' ';
        std::cout << result[degree];
    }
    std::cout << '\n';
}
