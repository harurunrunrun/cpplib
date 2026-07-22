// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/matrix_product_mod_2

#include <bitset>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/math/linear_algebra/matrix_product_mod_2.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t rows;
    std::size_t inner_size;
    std::size_t columns;
    std::cin >> rows >> inner_size >> columns;
    constexpr std::size_t capacity = 4096;
    std::vector<std::bitset<capacity>> lhs(rows);
    std::vector<std::bitset<capacity>> rhs(inner_size);
    std::string line;
    for(std::size_t row = 0; row < rows; ++row){
        std::cin >> line;
        for(std::size_t column = 0; column < inner_size; ++column){
            lhs[row].set(column, line[column] == '1');
        }
    }
    for(std::size_t row = 0; row < inner_size; ++row){
        std::cin >> line;
        for(std::size_t column = 0; column < columns; ++column){
            rhs[row].set(column, line[column] == '1');
        }
    }
    const auto result = matrix_product_mod_2<capacity, capacity>(
        lhs, rhs, inner_size, columns
    );
    for(const auto& row: result){
        for(std::size_t column = 0; column < columns; ++column){
            std::cout << static_cast<int>(row.test(column));
        }
        std::cout << '\n';
    }
}
