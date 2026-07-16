// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/structure/other/noncommutative_prefix_product.hpp"
#include "../../src/structure/types/monoid.hpp"

constexpr long long MOD = 1000000007LL;
using Matrix2 = std::array<long long, 4>;

constexpr Matrix2 matrix_multiply(Matrix2 left, Matrix2 right){
    return {
        (left[0] * right[0] + left[1] * right[2]) % MOD,
        (left[0] * right[1] + left[1] * right[3]) % MOD,
        (left[2] * right[0] + left[3] * right[2]) % MOD,
        (left[2] * right[1] + left[3] * right[3]) % MOD
    };
}

constexpr Matrix2 matrix_identity(){ return {1, 0, 0, 1}; }
constexpr auto matrix_monoid = Monoid<matrix_multiply, matrix_identity>{};

template<class Function>
bool throws_runtime_error(Function&& function){
    try{
        function();
    }catch(const std::runtime_error&){
        return true;
    }
    return false;
}

Matrix2 naive_product(const std::vector<Matrix2>& values, int left, int right){
    Matrix2 result = matrix_identity();
    for(int index = left; index < right; ++index){
        result = matrix_multiply(result, values[static_cast<std::size_t>(index)]);
    }
    return result;
}

void self_test(){
    NoncommutativePrefixProduct<matrix_monoid, 0> zero(0);
    assert(zero.empty());
    assert(zero.all_product() == matrix_identity());
    assert(throws_runtime_error([&]{ (void)zero.get(0); }));
    assert(throws_runtime_error([]{
        (void)NoncommutativePrefixProduct<matrix_monoid, 0>(1);
    }));

    const Matrix2 left{1, 2, 3, 4};
    const Matrix2 right{0, 1, 1, 0};
    assert(matrix_multiply(left, right) != matrix_multiply(right, left));
    NoncommutativePrefixProduct<matrix_monoid, 8> small(
        std::vector<Matrix2>{left, right}
    );
    assert(small.range_product(0, 2) == matrix_multiply(left, right));
    assert(small.prefix_product(1) == left);
    assert(small.prefix_product(0) == matrix_identity());
    small.set(0, right);
    assert(small.all_product() == matrix_multiply(right, right));
    assert(throws_runtime_error([&]{ small.set(-1, left); }));
    assert(throws_runtime_error([&]{ (void)small.get(2); }));
    assert(throws_runtime_error([&]{ (void)small.range_product(1, 0); }));
    assert(throws_runtime_error([&]{ (void)small.prefix_product(3); }));

    std::mt19937_64 random(2026071612);
    for(int iteration = 0; iteration < 2000; ++iteration){
        const int size = static_cast<int>(random() % 80);
        std::vector<Matrix2> values(static_cast<std::size_t>(size));
        for(Matrix2& matrix: values){
            for(long long& value: matrix) value = static_cast<long long>(random() % MOD);
        }
        NoncommutativePrefixProduct<matrix_monoid, 80> product(values);
        for(int operation = 0; operation < 300; ++operation){
            if(size != 0 && random() % 3 == 0){
                const int index = static_cast<int>(random() % static_cast<unsigned>(size));
                Matrix2 matrix{};
                for(long long& value: matrix) value = static_cast<long long>(random() % MOD);
                values[static_cast<std::size_t>(index)] = matrix;
                product.set(index, matrix);
                assert(product.get(index) == matrix);
            }else{
                const int left_index = static_cast<int>(random() % (size + 1U));
                const int right_index = left_index + static_cast<int>(
                    random() % static_cast<unsigned>(size - left_index + 1)
                );
                assert(product.range_product(left_index, right_index)
                    == naive_product(values, left_index, right_index));
                assert(product.prefix_product(right_index)
                    == naive_product(values, 0, right_index));
                assert(product.all_product() == naive_product(values, 0, size));
            }
        }
    }
}

void print_matrix(const Matrix2& matrix){
    std::cout << matrix[0] << ' ' << matrix[1] << ' '
        << matrix[2] << ' ' << matrix[3] << '\n';
}

int main(){
    int size;
    int query_count;
    if(!(std::cin >> size >> query_count)){
        self_test();
        return 0;
    }
    std::vector<Matrix2> values(static_cast<std::size_t>(size));
    for(Matrix2& matrix: values){
        std::cin >> matrix[0] >> matrix[1] >> matrix[2] >> matrix[3];
    }
    NoncommutativePrefixProduct<matrix_monoid, 4096> product(values);
    for(int query = 0; query < query_count; ++query){
        char type;
        std::cin >> type;
        if(type == 'S'){
            int index;
            Matrix2 matrix{};
            std::cin >> index >> matrix[0] >> matrix[1] >> matrix[2] >> matrix[3];
            product.set(index, matrix);
        }else if(type == 'G'){
            int index;
            std::cin >> index;
            print_matrix(product.get(index));
        }else if(type == 'P'){
            int right_index;
            std::cin >> right_index;
            print_matrix(product.prefix_product(right_index));
        }else if(type == 'R'){
            int left_index;
            int right_index;
            std::cin >> left_index >> right_index;
            print_matrix(product.range_product(left_index, right_index));
        }else{
            print_matrix(product.all_product());
        }
    }
}
