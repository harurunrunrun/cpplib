// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/linear_algebra/hafnian.hpp"
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

    const mint answer = hafnian(matrix);
    if(size <= 12){
        Matrix<mint, 40, 40> fixed_matrix(size, size);
        for(int row = 0; row < size; ++row){
            for(int column = 0; column < size; ++column){
                fixed_matrix(row, column) = matrix[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(column)];
            }
        }
        assert(hafnian(fixed_matrix) == answer);
    }
    std::cout << answer << '\n';
}
