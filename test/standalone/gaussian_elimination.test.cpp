// competitive-verifier: STANDALONE

#include <bitset>
#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../../src/algorithm/math/gaussian_elimination.hpp"
#include "../../src/structure/modint/modint.hpp"

using mint = Modint998244353;

template<class T>
bool satisfies(
    const std::vector<std::vector<T>>& matrix,
    const std::vector<T>& x,
    const std::vector<T>& rhs
){
    for(std::size_t i = 0; i < matrix.size(); ++i){
        T value{};
        for(std::size_t j = 0; j < x.size(); ++j) value += matrix[i][j] * x[j];
        if(value != rhs[i]) return false;
    }
    return true;
}

void self_test(){
    {
        Matrix<mint, 4, 4> matrix({{1, 2, 3}, {2, 4, 6}, {1, 1, 1}});
        assert(gaussian_rank(matrix) == 2);
        assert(gaussian_determinant(matrix) == mint(0));
        assert(!gaussian_inverse(matrix));
    }
    {
        std::vector<std::vector<mint>> matrix = {{2, 1}, {5, 3}};
        assert(gaussian_rank(matrix) == 2);
        assert(gaussian_determinant(matrix) == mint(1));
        auto inverse = gaussian_inverse(matrix);
        assert(inverse);
        for(int i = 0; i < 2; ++i){
            for(int j = 0; j < 2; ++j){
                mint value{};
                for(int k = 0; k < 2; ++k) value += matrix[i][k] * (*inverse)[k][j];
                assert(value == mint(i == j));
            }
        }
    }
    {
        std::vector<std::vector<mint>> matrix = {{1, 2, 3}, {2, 4, 6}};
        std::vector<mint> rhs = {4, 8};
        auto solution = solve_linear_system(matrix, rhs);
        assert(solution.consistent);
        assert(solution.nullspace_basis.size() == 2);
        assert(satisfies(matrix, solution.particular, rhs));
        std::vector<mint> zero(rhs.size());
        for(const auto& basis: solution.nullspace_basis) assert(satisfies(matrix, basis, zero));
        rhs[1] = 9;
        assert(!solve_linear_system(matrix, rhs).consistent);
    }
    {
        Matrix<mint, 2, 2> empty(0, 0);
        assert(gaussian_rank(empty) == 0);
        assert(gaussian_determinant(empty) == mint(1));
        auto inverse = gaussian_inverse(empty);
        assert(inverse && inverse->rows() == 0);
    }

    std::mt19937 rng(20260902);
    for(int rows = 0; rows <= 8; ++rows){
        for(int cols = 0; cols <= 8; ++cols){
            for(int step = 0; step < 40; ++step){
                Matrix<mint, 8, 8> fixed(rows, cols);
                std::vector<std::vector<mint>> dynamic(
                    static_cast<std::size_t>(rows),
                    std::vector<mint>(static_cast<std::size_t>(cols))
                );
                for(int i = 0; i < rows; ++i){
                    for(int j = 0; j < cols; ++j){
                        mint value = static_cast<int>(rng() % 11) - 5;
                        fixed(i, j) = value;
                        dynamic[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] = value;
                    }
                }
                assert(gaussian_rank(fixed) == gaussian_rank(dynamic));
                std::vector<mint> rhs(static_cast<std::size_t>(rows));
                for(auto& value: rhs) value = static_cast<int>(rng() % 11) - 5;
                auto lhs = solve_linear_system(fixed, rhs);
                auto rhs_solution = solve_linear_system(dynamic, rhs);
                assert(lhs.consistent == rhs_solution.consistent);
                if(lhs.consistent){
                    assert(satisfies(dynamic, lhs.particular, rhs));
                    assert(lhs.nullspace_basis.size() == static_cast<std::size_t>(cols - gaussian_rank(dynamic)));
                }
            }
        }
    }

    constexpr int MAX_BITS = 16;
    for(int rows = 0; rows <= 8; ++rows){
        for(int cols = 0; cols <= 8; ++cols){
            for(int step = 0; step < 80; ++step){
                std::vector<std::bitset<MAX_BITS>> matrix(static_cast<std::size_t>(rows));
                std::vector<int> rhs(static_cast<std::size_t>(rows));
                for(int i = 0; i < rows; ++i){
                    for(int j = 0; j < cols; ++j) matrix[static_cast<std::size_t>(i)][j] = rng() & 1;
                    rhs[static_cast<std::size_t>(i)] = rng() & 1;
                }
                auto solution = gf2_solve_linear_system(matrix, rhs, cols);
                bool brute_consistent = false;
                for(int mask = 0; mask < (1 << cols); ++mask){
                    bool ok = true;
                    for(int i = 0; i < rows; ++i){
                        int value = 0;
                        for(int j = 0; j < cols; ++j){
                            value ^= matrix[static_cast<std::size_t>(i)][j] && ((mask >> j) & 1);
                        }
                        if(value != rhs[static_cast<std::size_t>(i)]) ok = false;
                    }
                    brute_consistent |= ok;
                }
                assert(solution.consistent == brute_consistent);
                if(solution.consistent){
                    assert(solution.nullspace_basis.size() ==
                           static_cast<std::size_t>(cols - gf2_rank(matrix, cols)));
                }
            }
        }
    }
}

template<class T>
void print_vector(const std::vector<T>& values){
    for(std::size_t i = 0; i < values.size(); ++i){
        if(i != 0) std::cout << ' ';
        std::cout << values[i];
    }
    std::cout << '\n';
}

template<std::size_t MAX_COL>
void print_bits(const std::bitset<MAX_COL>& value, int columns){
    for(int j = 0; j < columns; ++j) std::cout << static_cast<int>(value[j]);
    std::cout << '\n';
}

int main(){
    int query_count;
    if(!(std::cin >> query_count)){
        self_test();
        return 0;
    }
    constexpr int MAX_BITS = 256;
    while(query_count--){
        std::string operation;
        int rows, cols;
        std::cin >> operation >> rows >> cols;
        if(operation == "G2RANK" || operation == "G2SOLVE"){
            std::vector<std::bitset<MAX_BITS>> matrix(static_cast<std::size_t>(rows));
            for(int i = 0; i < rows; ++i){
                std::string line;
                std::cin >> line;
                for(int j = 0; j < cols; ++j) matrix[static_cast<std::size_t>(i)][j] = line[static_cast<std::size_t>(j)] == '1';
            }
            if(operation == "G2RANK"){
                std::cout << gf2_rank(matrix, cols) << '\n';
            }else{
                std::string line;
                std::cin >> line;
                std::vector<int> rhs(static_cast<std::size_t>(rows));
                for(int i = 0; i < rows; ++i) rhs[static_cast<std::size_t>(i)] = line[static_cast<std::size_t>(i)] - '0';
                auto solution = gf2_solve_linear_system(matrix, rhs, cols);
                if(!solution.consistent){
                    std::cout << -1 << '\n';
                }else{
                    std::cout << solution.nullspace_basis.size() << '\n';
                    print_bits(solution.particular, cols);
                    for(const auto& basis: solution.nullspace_basis) print_bits(basis, cols);
                }
            }
            continue;
        }

        std::vector<std::vector<mint>> matrix(
            static_cast<std::size_t>(rows),
            std::vector<mint>(static_cast<std::size_t>(cols))
        );
        for(auto& row: matrix) for(auto& value: row) std::cin >> value;
        if(operation == "RANK"){
            std::cout << gaussian_rank(matrix) << '\n';
        }else if(operation == "DET"){
            std::cout << gaussian_determinant(matrix) << '\n';
        }else if(operation == "INV"){
            auto inverse = gaussian_inverse(matrix);
            if(!inverse){
                std::cout << -1 << '\n';
            }else{
                for(const auto& row: *inverse) print_vector(row);
            }
        }else{
            std::vector<mint> rhs(static_cast<std::size_t>(rows));
            for(auto& value: rhs) std::cin >> value;
            auto solution = solve_linear_system(matrix, rhs);
            if(!solution.consistent){
                std::cout << -1 << '\n';
            }else{
                std::cout << solution.nullspace_basis.size() << '\n';
                print_vector(solution.particular);
                for(const auto& basis: solution.nullspace_basis) print_vector(basis);
            }
        }
    }
}
