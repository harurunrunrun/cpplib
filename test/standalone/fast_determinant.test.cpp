// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/matrix_tree_theorem.hpp"
#include "../../src/algorithm/math/linear_algebra/fast_determinant.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

using Mint = FastModint998244353;

void self_test(){
    assert(fast_determinant(std::vector<std::vector<Mint>>{}) == Mint(1));
    assert(fast_determinant(std::vector<std::vector<Mint>>{{7}}) == Mint(7));
    assert(fast_determinant(std::vector<std::vector<Mint>>{
        {0, 1}, {1, 0}
    }) == Mint(-1));
    assert(fast_determinant(std::vector<std::vector<Mint>>{
        {0, 1, 0}, {0, 0, 1}, {1, 0, 0}
    }) == Mint(1));
    assert(fast_determinant(std::vector<std::vector<Mint>>{
        {1, 2, 3}, {2, 4, 6}, {7, 8, 9}
    }) == Mint(0));

    bool thrown = false;
    try{
        (void)fast_determinant(std::vector<Mint>(5), 2);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)fast_determinant(
            std::vector<std::vector<Mint>>{{1, 2}, {3}}
        );
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        const Matrix<Mint, 3, 4> nonsquare(2, 3);
        (void)fast_determinant(nonsquare);
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
    std::vector<std::vector<Mint>> matrix(
        static_cast<std::size_t>(n),
        std::vector<Mint>(static_cast<std::size_t>(n))
    );
    for(auto& row: matrix){
        for(Mint& value: row) std::cin >> value;
    }
    const Mint result = fast_determinant(matrix);

    if(n <= 32){
        MatrixTreeFixedMatrix<Mint, 32> fixed{};
        Matrix<Mint, 32, 32> dynamic_fixed(n, n);
        for(int row = 0; row < n; ++row){
            for(int column = 0; column < n; ++column){
                const Mint value = matrix[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(column)];
                fixed[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(column)] = value;
                dynamic_fixed(row, column) = value;
            }
        }
        assert((result == matrix_tree_determinant<Mint, 32>(fixed, n)));
        assert(result == fast_determinant(dynamic_fixed));
    }
    if(n >= 256){
        assert(
            static_cast<std::size_t>(128 * 128 * 128) >
            fast_matrix_multiply_internal::strassen_scalar_product_budget(128)
        );
    }
    std::cout << result << '\n';
}
