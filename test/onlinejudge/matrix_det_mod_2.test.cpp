// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/matrix_det_mod_2

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/math/linear_algebra/gf2_determinant.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<std::string> matrix(static_cast<std::size_t>(size));
    for(std::string& row: matrix) std::cin >> row;
    std::cout << gf2_determinant(matrix) << '\n';
}
