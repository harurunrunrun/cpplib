// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/adjugate_matrix.hpp"
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

    const auto result = adjugate_matrix(matrix);
    if(size <= 16){
        Matrix<mint, 16, 16> fixed_matrix(size, size);
        for(int row = 0; row < size; ++row){
            for(int column = 0; column < size; ++column){
                fixed_matrix(row, column) = matrix[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(column)];
            }
        }
        const auto fixed_result = adjugate_matrix(fixed_matrix);
        for(int row = 0; row < size; ++row){
            for(int column = 0; column < size; ++column){
                assert(fixed_result(row, column) == result[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(column)]);
            }
        }
    }
    for(const auto& row: result){
        for(std::size_t column = 0; column < row.size(); ++column){
            if(column != 0) std::cout << ' ';
            std::cout << row[column];
        }
        std::cout << '\n';
    }
}
