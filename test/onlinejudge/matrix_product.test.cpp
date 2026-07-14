// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/matrix_product

#include <iostream>
#include <memory>

#include "../../src/structure/matrix/matrix.hpp"
#include "../../src/structure/modint/modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    using mint = Modint<998244353>;
    int rows, inner, columns;
    std::cin >> rows >> inner >> columns;
    auto left = std::make_unique<Matrix<mint, 500, 500>>(rows, inner);
    auto right = std::make_unique<Matrix<mint, 500, 500>>(inner, columns);
    for(int row = 0; row < rows; ++row){
        for(int column = 0; column < inner; ++column){
            std::cin >> (*left)(row, column);
        }
    }
    for(int row = 0; row < inner; ++row){
        for(int column = 0; column < columns; ++column){
            std::cin >> (*right)(row, column);
        }
    }
    const auto product = *left * *right;
    for(int row = 0; row < rows; ++row){
        for(int column = 0; column < columns; ++column){
            if(column != 0) std::cout << ' ';
            std::cout << product(row, column);
        }
        std::cout << '\n';
    }
}
