// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/system_of_linear_equations_mod_2

#include <bitset>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/math/linear_algebra/gaussian_elimination.hpp"

template<std::size_t MAX_COL>
void print_bits(const std::bitset<MAX_COL>& value, int columns){
    for(int j = 0; j < columns; ++j) std::cout << static_cast<int>(value[j]);
    std::cout << '\n';
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr std::size_t MAX_COL = 4096;
    int rows, cols;
    std::cin >> rows >> cols;
    std::vector<std::bitset<MAX_COL>> matrix(static_cast<std::size_t>(rows));
    for(int i = 0; i < rows; ++i){
        std::string line;
        std::cin >> line;
        for(int j = 0; j < cols; ++j){
            matrix[static_cast<std::size_t>(i)][j] = line[static_cast<std::size_t>(j)] == '1';
        }
    }
    std::string line;
    std::cin >> line;
    std::vector<int> rhs(static_cast<std::size_t>(rows));
    for(int i = 0; i < rows; ++i){
        rhs[static_cast<std::size_t>(i)] = line[static_cast<std::size_t>(i)] == '1';
    }

    auto solution = gf2_solve_linear_system(matrix, rhs, cols);
    if(!solution.consistent){
        std::cout << -1 << '\n';
        return 0;
    }
    std::cout << solution.nullspace_basis.size() << '\n';
    print_bits(solution.particular, cols);
    for(const auto& basis: solution.nullspace_basis) print_bits(basis, cols);
}
