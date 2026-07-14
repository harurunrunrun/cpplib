// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/matrix_rank_mod_2

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/math/gf2_matrix_rank.hpp"


int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int rows, cols;
    std::cin >> rows >> cols;
    std::vector<std::string> matrix(static_cast<std::size_t>(rows));
    for(std::string& row: matrix) std::cin >> row;
    std::cout << gf2_matrix_rank(
        matrix, static_cast<std::size_t>(cols)
    ) << '\n';
}
