// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/linear_algebra/adjugate_matrix.hpp"
#include "../../src/algorithm/math/linear_algebra/fast_adjugate_matrix.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

using Mint = FastModint998244353;

void self_test(){
    assert(fast_adjugate_matrix(std::vector<std::vector<Mint>>{}).empty());
    assert(fast_adjugate_matrix(std::vector<Mint>{}, 0).empty());
    assert(fast_adjugate_matrix(std::vector<std::vector<Mint>>{{0}}) ==
        std::vector<std::vector<Mint>>{{1}});
    assert(fast_adjugate_matrix(std::vector<std::vector<Mint>>{{7}}) ==
        std::vector<std::vector<Mint>>{{1}});
    assert(fast_adjugate_matrix(std::vector<std::vector<Mint>>{
        {1, 2}, {3, 4}
    }) == std::vector<std::vector<Mint>>({{4, -2}, {-3, 1}}));
    assert(fast_adjugate_matrix(std::vector<std::vector<Mint>>{
        {0, 1, 0}, {0, 0, 1}, {0, 0, 0}
    }) == std::vector<std::vector<Mint>>({
        {0, 0, 1}, {0, 0, 0}, {0, 0, 0}
    }));
    assert(fast_adjugate_matrix(std::vector<std::vector<Mint>>{
        {1, 0, 0}, {0, 0, 0}, {0, 0, 0}
    }) == std::vector<std::vector<Mint>>(3, std::vector<Mint>(3)));

    bool thrown = false;
    try{
        (void)fast_adjugate_matrix(std::vector<Mint>(5), 2);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)fast_adjugate_matrix(
            std::vector<std::vector<Mint>>{{1, 2}, {3}}
        );
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        const Matrix<Mint, 3, 4> nonsquare(2, 3);
        (void)fast_adjugate_matrix(nonsquare);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    self_test();

    int n;
    std::cin >> n;
    assert(n >= 0);
    const std::size_t size = static_cast<std::size_t>(n);
    std::vector<std::vector<Mint>> matrix(
        size, std::vector<Mint>(size)
    );
    std::vector<Mint> flat(size * size);
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            std::cin >> matrix[row][column];
            flat[row * size + column] = matrix[row][column];
        }
    }

    const auto result = fast_adjugate_matrix(matrix);
    const auto flat_result = fast_adjugate_matrix(flat, size);
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            assert(result[row][column] == flat_result[row * size + column]);
        }
    }

    if(n <= 32){
        assert(result == adjugate_matrix(matrix));
    }
    if(n <= 16){
        Matrix<Mint, 16, 16> fixed(n, n);
        for(int row = 0; row < n; ++row){
            for(int column = 0; column < n; ++column){
                fixed(row, column) = matrix[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(column)];
            }
        }
        const auto fixed_result = fast_adjugate_matrix(fixed);
        for(int row = 0; row < n; ++row){
            for(int column = 0; column < n; ++column){
                assert(fixed_result(row, column) ==
                    result[static_cast<std::size_t>(row)]
                        [static_cast<std::size_t>(column)]);
            }
        }
    }
    if(n >= 256){
        assert(
            static_cast<std::size_t>(128 * 128 * 128) >
            fast_matrix_multiply_internal::strassen_scalar_product_budget(128)
        );
    }

    for(const auto& row: result){
        for(std::size_t column = 0; column < row.size(); ++column){
            if(column != 0) std::cout << ' ';
            std::cout << row[column];
        }
        std::cout << '\n';
    }
}
