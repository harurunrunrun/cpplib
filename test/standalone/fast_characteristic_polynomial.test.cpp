// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/characteristic_polynomial.hpp"
#include "../../src/algorithm/math/fast_characteristic_polynomial.hpp"
#include "../../src/structure/modint/modint.hpp"

using mint = Modint998244353;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    bool vector_exception = false;
    try{
        (void)fast_characteristic_polynomial(
            std::vector<std::vector<mint>>{{mint(1), mint(2)}}
        );
    }catch(const std::invalid_argument&){
        vector_exception = true;
    }
    assert(vector_exception);

    bool fixed_exception = false;
    try{
        const Matrix<mint, 2, 3> nonsquare(1, 2);
        (void)fast_characteristic_polynomial(nonsquare);
    }catch(const std::invalid_argument&){
        fixed_exception = true;
    }
    assert(fixed_exception);
    {
        using binary = Modint<2>;
        std::vector<std::vector<binary>> binary_jordan(
            5, std::vector<binary>(5)
        );
        for(std::size_t index = 0; index < 5; ++index){
            binary_jordan[index][index] = binary(1);
            if(index + 1 < 5) binary_jordan[index + 1][index] = binary(1);
        }
        assert(fast_characteristic_polynomial(binary_jordan) ==
            characteristic_polynomial(binary_jordan));
    }
    {
        using small_field = Modint<5>;
        std::vector<std::vector<small_field>> small_characteristic(
            6, std::vector<small_field>(6)
        );
        for(std::size_t row = 0; row < 6; ++row){
            for(std::size_t column = 0; column < 6; ++column){
                small_characteristic[row][column] = small_field(
                    static_cast<long long>(row * 11 + column * 7 + row * column)
                );
            }
        }
        assert(fast_characteristic_polynomial(small_characteristic) ==
            characteristic_polynomial(small_characteristic));
    }

    int size;
    std::cin >> size;
    std::vector<std::vector<mint>> matrix(
        static_cast<std::size_t>(size),
        std::vector<mint>(static_cast<std::size_t>(size))
    );
    for(auto& row: matrix){
        for(mint& value: row) std::cin >> value;
    }

    const std::vector<mint> result = fast_characteristic_polynomial(matrix);
    if(size <= 40){
        assert(result == characteristic_polynomial(matrix));
    }
    if(size <= 128){
        Matrix<mint, 128, 128> fixed(size, size);
        for(int row = 0; row < size; ++row){
            for(int column = 0; column < size; ++column){
                fixed(row, column) = matrix[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(column)];
            }
        }
        assert(result == fast_characteristic_polynomial(fixed));
    }

    for(std::size_t degree = 0; degree < result.size(); ++degree){
        if(degree != 0) std::cout << ' ';
        std::cout << result[degree];
    }
    std::cout << '\n';
}
