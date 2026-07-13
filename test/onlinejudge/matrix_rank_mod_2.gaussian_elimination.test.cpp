// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/matrix_rank_mod_2

#include <bitset>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/math/gaussian_elimination.hpp"

template<std::size_t WIDTH>
int read_and_rank(int rows, int cols, bool transpose){
    if(!transpose){
        std::vector<std::bitset<WIDTH>> matrix(static_cast<std::size_t>(rows));
        for(int i = 0; i < rows; ++i){
            std::string line;
            std::cin >> line;
            for(int j = 0; j < cols; ++j){
                matrix[static_cast<std::size_t>(i)][j] = line[static_cast<std::size_t>(j)] == '1';
            }
        }
        return gf2_rank(matrix, cols);
    }

    std::vector<std::bitset<WIDTH>> matrix(static_cast<std::size_t>(cols));
    for(int i = 0; i < rows; ++i){
        std::string line;
        std::cin >> line;
        for(int j = 0; j < cols; ++j){
            matrix[static_cast<std::size_t>(j)][i] = line[static_cast<std::size_t>(j)] == '1';
        }
    }
    return gf2_rank(matrix, rows);
}

template<std::size_t WIDTH>
int dispatch_rank(int rows, int cols, bool transpose, int short_side){
    if(short_side <= static_cast<int>(WIDTH)) return read_and_rank<WIDTH>(rows, cols, transpose);
    if constexpr(WIDTH < 4096) return dispatch_rank<2 * WIDTH>(rows, cols, transpose, short_side);
    throw std::runtime_error("library assertion fault: size violation (matrix_rank_mod_2 test).");
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int rows, cols;
    std::cin >> rows >> cols;
    bool transpose = rows < cols;
    int short_side = transpose ? rows : cols;
    std::cout << dispatch_rank<64>(rows, cols, transpose, short_side) << '\n';
}
