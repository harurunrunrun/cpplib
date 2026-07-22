// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/adjugate_matrix

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/linear_algebra/adjugate_matrix.hpp"
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
    const auto result = adjugate_matrix(matrix);
    for(const auto& row: result){
        for(std::size_t column = 0; column < row.size(); ++column){
            if(column != 0) std::cout << ' ';
            std::cout << row[column];
        }
        std::cout << '\n';
    }
}
