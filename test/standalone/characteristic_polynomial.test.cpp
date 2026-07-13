// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/characteristic_polynomial.hpp"
#include "../../src/algorithm/math/characteristic_polynomial.hpp"
#include "../../src/structure/modint/modint.hpp"

using mint = Modint998244353;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<std::vector<mint>> matrix(
        static_cast<std::size_t>(size),
        std::vector<mint>(static_cast<std::size_t>(size))
    );
    for(auto& row: matrix) for(mint& value: row) std::cin >> value;

    if(size <= 64){
        Matrix<mint, 64, 64> fixed_matrix(size, size);
        for(int row = 0; row < size; ++row){
            for(int column = 0; column < size; ++column){
                fixed_matrix(row, column) = matrix[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(column)];
            }
        }
        assert(characteristic_polynomial(fixed_matrix) ==
            characteristic_polynomial(matrix));
    }

    const auto result = characteristic_polynomial(std::move(matrix));
    for(std::size_t degree = 0; degree < result.size(); ++degree){
        if(degree != 0) std::cout << ' ';
        std::cout << result[degree];
    }
    std::cout << '\n';
}
