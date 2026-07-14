// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/matrix_det_mod_2

#include <bitset>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/math/gaussian_elimination.hpp"

template<std::size_t WIDTH>
int determinant(int size){
    if(size <= static_cast<int>(WIDTH)){
        std::vector<std::bitset<WIDTH>> matrix(
            static_cast<std::size_t>(size)
        );
        for(int row = 0; row < size; ++row){
            std::string line;
            std::cin >> line;
            for(int column = 0; column < size; ++column){
                matrix[static_cast<std::size_t>(row)][
                    static_cast<std::size_t>(column)
                ] = line[static_cast<std::size_t>(column)] == '1';
            }
        }
        return gf2_rank(matrix, size) == size ? 1 : 0;
    }
    if constexpr(WIDTH < 4096) return determinant<2 * WIDTH>(size);
    throw std::runtime_error(
        "library assertion fault: size violation (matrix_det_mod_2 test)."
    );
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::cout << determinant<64>(size) << '\n';
}
