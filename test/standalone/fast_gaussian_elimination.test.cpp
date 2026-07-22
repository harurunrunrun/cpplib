// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/fast_gaussian_elimination.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

using Mint = FastModint998244353;

template<class T>
void assert_same_solution(
    const GaussianLinearSystemSolution<T>& left,
    const GaussianLinearSystemSolution<T>& right
){
    assert(left.consistent == right.consistent);
    assert(left.particular == right.particular);
    assert(left.nullspace_basis == right.nullspace_basis);
    assert(left.pivot_columns == right.pivot_columns);
}

template<class T, int MAX_ROW, int MAX_COL>
void assert_matrix_equals(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix,
    const std::vector<std::vector<T>>& expected
){
    assert(matrix.rows() == static_cast<int>(expected.size()));
    const int columns = expected.empty() ? 0 :
        static_cast<int>(expected.front().size());
    assert(matrix.cols() == columns);
    for(int row = 0; row < matrix.rows(); ++row){
        for(int column = 0; column < matrix.cols(); ++column){
            assert(matrix(row, column) == expected[static_cast<std::size_t>(row)]
                [static_cast<std::size_t>(column)]);
        }
    }
}

void self_test(){
    assert(fast_gaussian_rank(std::vector<Mint>{}, 0, 0) == 0);
    const auto empty_solution = fast_solve_linear_system(
        std::vector<Mint>{}, 0, 0, std::vector<Mint>{}
    );
    assert(empty_solution.consistent);
    assert(empty_solution.particular.empty());
    assert(empty_solution.nullspace_basis.empty());
    const auto empty_inverse = fast_gaussian_inverse(
        std::vector<Mint>{}, 0
    );
    assert(empty_inverse && empty_inverse->empty());
    assert(fast_gaussian_determinant(std::vector<Mint>{}, 0) == Mint(1));

    Matrix<Mint, 4, 6> no_rows(0, 5);
    assert(fast_gaussian_rank(no_rows) == 0);
    const auto no_row_solution = fast_solve_linear_system(
        no_rows, std::vector<Mint>{}
    );
    assert(no_row_solution.consistent);
    assert(no_row_solution.particular == std::vector<Mint>(5));
    assert(no_row_solution.nullspace_basis.size() == 5);
    for(std::size_t index = 0; index < 5; ++index){
        std::vector<Mint> expected(5);
        expected[index] = Mint(1);
        assert(no_row_solution.nullspace_basis[index] == expected);
    }

    bool thrown = false;
    try{
        (void)fast_gaussian_rank(std::vector<Mint>(3), 2, 2);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)fast_gaussian_rank(
            std::vector<std::vector<Mint>>{{1, 2}, {3}}
        );
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)fast_solve_linear_system(
            std::vector<Mint>{1, 2}, 1, 2, std::vector<Mint>{}
        );
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)fast_gaussian_inverse(
            std::vector<std::vector<Mint>>{{1, 2}}
        );
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)fast_gaussian_rank(
            std::vector<Mint>{},
            std::numeric_limits<std::size_t>::max(),
            2
        );
    }catch(const std::length_error&){
        thrown = true;
    }
    assert(thrown);
}

void check_case(
    const std::vector<std::vector<Mint>>& matrix,
    const std::vector<Mint>& right_hand_side,
    bool check_all_overloads
){
    const std::size_t rows = matrix.size();
    const std::size_t columns = rows == 0 ? 0 : matrix.front().size();
    std::vector<Mint> flat(rows * columns);
    for(std::size_t row = 0; row < rows; ++row){
        for(std::size_t column = 0; column < columns; ++column){
            flat[row * columns + column] = matrix[row][column];
        }
    }

    const int expected_rank = gaussian_rank(matrix);
    const auto expected_solution = solve_linear_system(
        matrix, right_hand_side
    );
    assert(fast_gaussian_rank(flat, rows, columns) ==
        static_cast<std::size_t>(expected_rank));
    assert_same_solution(
        fast_solve_linear_system(
            flat, rows, columns, right_hand_side
        ),
        expected_solution
    );
    if(check_all_overloads){
        assert(fast_gaussian_rank(matrix) ==
            static_cast<std::size_t>(expected_rank));
        assert_same_solution(
            fast_solve_linear_system(matrix, right_hand_side),
            expected_solution
        );
    }

    Matrix<Mint, 260, 260> fixed(
        static_cast<int>(rows), static_cast<int>(columns)
    );
    for(std::size_t row = 0; row < rows; ++row){
        for(std::size_t column = 0; column < columns; ++column){
            fixed(static_cast<int>(row), static_cast<int>(column)) =
                matrix[row][column];
        }
    }
    if(check_all_overloads){
        assert(fast_gaussian_rank(fixed) ==
            static_cast<std::size_t>(expected_rank));
        assert_same_solution(
            fast_solve_linear_system(fixed, right_hand_side),
            expected_solution
        );
    }

    if(rows != columns) return;
    const Mint expected_determinant = gaussian_determinant(matrix);
    const auto expected_inverse = gaussian_inverse(matrix);
    assert(fast_gaussian_determinant(flat, rows) == expected_determinant);
    const auto flat_inverse = fast_gaussian_inverse(flat, rows);
    assert(flat_inverse.has_value() == expected_inverse.has_value());
    if(flat_inverse){
        for(std::size_t row = 0; row < rows; ++row){
            for(std::size_t column = 0; column < columns; ++column){
                assert((*flat_inverse)[row * columns + column] ==
                    (*expected_inverse)[row][column]);
            }
        }
    }
    if(!check_all_overloads) return;
    assert(fast_gaussian_determinant(matrix) == expected_determinant);
    assert(fast_gaussian_determinant(fixed) == expected_determinant);
    const auto nested_inverse = fast_gaussian_inverse(matrix);
    assert(nested_inverse == expected_inverse);
    const auto fixed_inverse = fast_gaussian_inverse(fixed);
    assert(fixed_inverse.has_value() == expected_inverse.has_value());
    if(fixed_inverse) assert_matrix_equals(*fixed_inverse, *expected_inverse);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    self_test();

    int test_count;
    std::cin >> test_count;
    assert(test_count >= 0);
    for(int test = 0; test < test_count; ++test){
        int rows;
        int columns;
        int check_all;
        std::cin >> rows >> columns >> check_all;
        assert(0 <= rows && rows <= 260);
        assert(0 <= columns && columns <= 260);
        std::vector<std::vector<Mint>> matrix(
            static_cast<std::size_t>(rows),
            std::vector<Mint>(static_cast<std::size_t>(columns))
        );
        for(auto& row: matrix){
            for(Mint& value: row) std::cin >> value;
        }
        std::vector<Mint> right_hand_side(static_cast<std::size_t>(rows));
        for(Mint& value: right_hand_side) std::cin >> value;
        check_case(matrix, right_hand_side, check_all != 0);
    }
    assert(
        fast_matrix_multiply_internal::strassen_scalar_product_budget(128) <
        static_cast<std::size_t>(128) * 128 * 128
    );
    std::cout << "OK\n";
}
