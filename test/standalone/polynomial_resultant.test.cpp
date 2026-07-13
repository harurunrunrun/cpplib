// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/math/polynomial_resultant.hpp"
#include "../../src/structure/modint/modint.hpp"

using mint = Modint998244353;

mint determinant(std::vector<std::vector<mint>> matrix){
    const int size = static_cast<int>(matrix.size());
    mint result = 1;
    for(int column = 0; column < size; ++column){
        int pivot = column;
        while(pivot < size && matrix[static_cast<std::size_t>(pivot)]
            [static_cast<std::size_t>(column)] == mint(0)){
            ++pivot;
        }
        if(pivot == size) return 0;
        if(pivot != column){
            std::swap(matrix[static_cast<std::size_t>(pivot)],
                matrix[static_cast<std::size_t>(column)]);
            result = -result;
        }
        const mint pivot_value = matrix[static_cast<std::size_t>(column)]
            [static_cast<std::size_t>(column)];
        result *= pivot_value;
        const mint inverse = pivot_value.inv();
        for(int row = column + 1; row < size; ++row){
            const mint factor = matrix[static_cast<std::size_t>(row)]
                [static_cast<std::size_t>(column)] * inverse;
            for(int entry = column; entry < size; ++entry){
                matrix[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(entry)] -=
                    factor * matrix[static_cast<std::size_t>(column)]
                        [static_cast<std::size_t>(entry)];
            }
        }
    }
    return result;
}

template<std::size_t CAPACITY>
mint sylvester_resultant(
    const std::array<mint, CAPACITY>& left,
    std::size_t left_count,
    const std::array<mint, CAPACITY>& right,
    std::size_t right_count
){
    while(left_count > 0 && left[left_count - 1] == mint(0)) --left_count;
    while(right_count > 0 && right[right_count - 1] == mint(0)) --right_count;
    if(left_count == 0 || right_count == 0) return 0;
    const std::size_t left_degree = left_count - 1;
    const std::size_t right_degree = right_count - 1;
    const std::size_t size = left_degree + right_degree;
    if(size == 0) return 1;
    std::vector<std::vector<mint>> matrix(
        size, std::vector<mint>(size)
    );
    for(std::size_t row = 0; row < right_degree; ++row){
        for(std::size_t degree = 0; degree <= left_degree; ++degree){
            matrix[row][row + left_degree - degree] = left[degree];
        }
    }
    for(std::size_t offset = 0; offset < left_degree; ++offset){
        const std::size_t row = right_degree + offset;
        for(std::size_t degree = 0; degree <= right_degree; ++degree){
            matrix[row][offset + right_degree - degree] = right[degree];
        }
    }
    return determinant(std::move(matrix));
}

void self_test(){
    {
        std::array<mint, 8> left{-1, 0, 1};
        std::array<mint, 8> right{-2, 1};
        assert(polynomial_resultant(left, 3, right, 2) == mint(3));
        assert(polynomial_resultant(right, 2, left, 3) == mint(3));
    }
    {
        std::array<mint, 8> left{0, 1};
        std::array<mint, 8> right{-5, 1};
        assert(polynomial_resultant(left, 2, right, 2) == mint(-5));
        assert(polynomial_resultant(right, 2, left, 2) == mint(5));
    }
    {
        std::array<mint, 8> left{1, 2, 0, 1};
        std::array<mint, 8> right{1, 0, 1};
        const auto remainder = polynomial_remainder(left, 4, right, 3);
        assert(remainder.coefficient_count == 2);
        assert(remainder.coefficients[0] == mint(1));
        assert(remainder.coefficients[1] == mint(1));
    }
    {
        std::array<mint, 8> quadratic{6, -5, 2};
        assert(polynomial_discriminant(quadratic, 3) == mint(-23));
        std::array<mint, 8> cubic{7, -2, 0, 1};
        assert(polynomial_discriminant(cubic, 4) == mint(-1291));
        std::array<mint, 8> repeated{1, -2, 1};
        assert(polynomial_discriminant(repeated, 3) == mint(0));
        std::array<mint, 8> constant{9};
        assert(polynomial_discriminant(constant, 1) == mint(1));
    }
    {
        std::array<mint, 8> zero{};
        std::array<mint, 8> constant{3};
        assert(polynomial_resultant(zero, 0, constant, 1) == mint(0));
        bool thrown = false;
        try{
            (void)polynomial_remainder(constant, 1, zero, 8);
        }catch(const std::invalid_argument&){
            thrown = true;
        }
        assert(thrown);
    }

    std::mt19937_64 random(2026071320);
    for(int iteration = 0; iteration < 10000; ++iteration){
        const std::size_t left_degree = random() % 7;
        const std::size_t right_degree = random() % 7;
        std::array<mint, 8> left{};
        std::array<mint, 8> right{};
        for(std::size_t degree = 0; degree <= left_degree; ++degree){
            left[degree] = mint(static_cast<long long>(random() % 101) - 50);
        }
        for(std::size_t degree = 0; degree <= right_degree; ++degree){
            right[degree] = mint(static_cast<long long>(random() % 101) - 50);
        }
        if(left[left_degree] == mint(0)) left[left_degree] = 1;
        if(right[right_degree] == mint(0)) right[right_degree] = 1;
        const mint expected = sylvester_resultant(
            left, left_degree + 1, right, right_degree + 1
        );
        const mint actual = polynomial_resultant(
            left, left_degree + 1, right, right_degree + 1
        );
        assert(actual == expected);
    }

    constexpr std::size_t capacity = 2005;
    static std::array<mint, capacity> large_left{};
    static std::array<mint, capacity> large_right{};
    large_left[0] = 3;
    large_left[2000] = 7;
    large_right[1500] = 1;
    assert(polynomial_resultant(
        large_left, 2001, large_right, 1501
    ) == mint(3).pow(1500));
}

int main(){
    std::size_t left_count;
    std::size_t right_count;
    if(!(std::cin >> left_count >> right_count)){
        self_test();
        return 0;
    }
    constexpr std::size_t capacity = 20005;
    static std::array<mint, capacity> left{};
    static std::array<mint, capacity> right{};
    for(std::size_t index = 0; index < left_count; ++index) std::cin >> left[index];
    for(std::size_t index = 0; index < right_count; ++index) std::cin >> right[index];
    std::cout << polynomial_resultant(
        left, left_count, right, right_count
    ) << '\n';
}
