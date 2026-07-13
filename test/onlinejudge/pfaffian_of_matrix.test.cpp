// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/pfaffian_of_matrix

#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/pfaffian.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int half_size;
    std::cin >> half_size;
    const int size = 2 * half_size;
    std::vector<std::vector<FastModint998244353>> matrix(
        static_cast<std::size_t>(size),
        std::vector<FastModint998244353>(static_cast<std::size_t>(size))
    );
    for(auto& row: matrix){
        for(FastModint998244353& value: row) std::cin >> value;
    }
    std::cout << pfaffian(std::move(matrix)) << '\n';
}
