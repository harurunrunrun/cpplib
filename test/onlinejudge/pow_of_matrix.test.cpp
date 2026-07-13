#define PROBLEM "https://judge.yosupo.jp/problem/pow_of_matrix"

#include <cstdint>
#include <iostream>
#include <memory>

#include "../../src/structure/matrix/square_matrix.hpp"
#include "../../src/structure/modint/modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using mint = Modint<998244353>;
    int size;
    std::uint64_t exponent;
    std::cin >> size >> exponent;
    auto matrix = std::make_unique<SquareMatrix<mint, 500>>(size);
    for(int row = 0; row < size; ++row){
        for(int column = 0; column < size; ++column){
            std::cin >> (*matrix)(row, column);
        }
    }
    const auto result = matrix->pow(static_cast<long long>(exponent));
    for(int row = 0; row < size; ++row){
        for(int column = 0; column < size; ++column){
            if(column != 0) std::cout << ' ';
            std::cout << result(row, column);
        }
        std::cout << '\n';
    }
}
