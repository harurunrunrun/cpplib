// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/matrix_det

#include <iostream>
#include <vector>

#include "../../src/algorithm/math/linear_algebra/gaussian_elimination.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<std::vector<FastModint998244353>> matrix(
        static_cast<std::size_t>(n),
        std::vector<FastModint998244353>(static_cast<std::size_t>(n))
    );
    for(auto& row: matrix) for(auto& value: row) std::cin >> value;
    std::cout << gaussian_determinant(matrix) << '\n';
}
