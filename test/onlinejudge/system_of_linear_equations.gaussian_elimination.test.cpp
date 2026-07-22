// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/system_of_linear_equations

#include <iostream>
#include <vector>

#include "../../src/algorithm/math/linear_algebra/gaussian_elimination.hpp"
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

    int rows, cols;
    std::cin >> rows >> cols;
    std::vector<std::vector<FastModint998244353>> matrix(
        static_cast<std::size_t>(rows),
        std::vector<FastModint998244353>(static_cast<std::size_t>(cols))
    );
    for(auto& row: matrix) for(auto& value: row) std::cin >> value;
    std::vector<FastModint998244353> rhs(static_cast<std::size_t>(rows));
    for(auto& value: rhs) std::cin >> value;

    auto solution = solve_linear_system(matrix, rhs);
    if(!solution.consistent){
        std::cout << -1 << '\n';
        return 0;
    }
    std::cout << solution.nullspace_basis.size() << '\n';
    print_vector(solution.particular);
    for(const auto& basis: solution.nullspace_basis) print_vector(basis);
}
