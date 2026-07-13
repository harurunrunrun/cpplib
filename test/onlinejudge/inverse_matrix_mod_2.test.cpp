// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/inverse_matrix_mod_2

#include <bitset>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/inverse_matrix_mod_2.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t size;
    std::cin >> size;
    constexpr std::size_t capacity = 4096;
    std::vector<std::bitset<capacity>> matrix(size);
    std::string line;
    for(std::size_t row = 0; row < size; ++row){
        std::cin >> line;
        for(std::size_t column = 0; column < size; ++column){
            matrix[row].set(column, line[column] == '1');
        }
    }
    const auto inverse = inverse_matrix_mod_2<capacity>(std::move(matrix), size);
    if(!inverse){
        std::cout << "-1\n";
        return 0;
    }
    for(const auto& row: *inverse){
        for(std::size_t column = 0; column < size; ++column){
            std::cout << static_cast<int>(row.test(column));
        }
        std::cout << '\n';
    }
}
