// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/inverse_matrix

#include <iostream>
#include <vector>

#include "../../src/algorithm/math/gaussian_elimination.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

template<class T>
void print_vector(const std::vector<T>& values){
    for(std::size_t i = 0; i < values.size(); ++i){
        if(i != 0) std::cout << ' ';
        std::cout << values[i];
    }
    std::cout << '\n';
}

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
    auto inverse = gaussian_inverse(matrix);
    if(!inverse){
        std::cout << -1 << '\n';
        return 0;
    }
    for(const auto& row: *inverse) print_vector(row);
}
