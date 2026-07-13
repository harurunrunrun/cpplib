// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/hafnian_of_matrix

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/hafnian.hpp"
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
    std::cout << hafnian(matrix) << '\n';
}
