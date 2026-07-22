// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/math/fast_matrix_multiply.hpp"

namespace{

template<class T>
std::vector<T> naive_matrix_multiply(
    const std::vector<T>& left,
    std::size_t rows,
    std::size_t inner,
    const std::vector<T>& right,
    std::size_t columns
){
    std::vector<T> output(rows * columns);
    for(std::size_t row = 0; row < rows; ++row){
        for(std::size_t middle = 0; middle < inner; ++middle){
            for(std::size_t column = 0; column < columns; ++column){
                output[row * columns + column] +=
                    left[row * inner + middle] *
                    right[middle * columns + column];
            }
        }
    }
    return output;
}

void print_matrix(
    std::size_t rows,
    std::size_t columns,
    const std::vector<long long>& values
){
    std::cout << rows << ' ' << columns;
    for(const long long value: values) std::cout << ' ' << value;
    std::cout << '\n';
}

struct ExactRing{
    long long value = 0;

    ExactRing() = default;
    explicit ExactRing(long long value_): value(value_){}

    ExactRing& operator+=(const ExactRing& other){
        value += other.value;
        return *this;
    }

    ExactRing& operator-=(const ExactRing& other){
        value -= other.value;
        return *this;
    }

    friend ExactRing operator+(ExactRing left, const ExactRing& right){
        left += right;
        return left;
    }

    friend ExactRing operator-(ExactRing left, const ExactRing& right){
        left -= right;
        return left;
    }

    friend ExactRing operator*(const ExactRing& left, const ExactRing& right){
        return ExactRing(left.value * right.value);
    }

    friend bool operator==(const ExactRing&, const ExactRing&) = default;
};

void self_test_contracts(){
    std::vector<ExactRing> identity{
        ExactRing(1), ExactRing(0), ExactRing(0), ExactRing(1)
    };
    std::vector<ExactRing> matrix{
        ExactRing(2), ExactRing(-3), ExactRing(5), ExactRing(7)
    };
    const auto product = fast_matrix_multiply(identity, 2, 2, matrix, 2);
    assert(product == matrix);
    Matrix<long long, 2, 3> capacity_left({{1, 2, 3}, {4, 5, 6}});
    Matrix<long long, 4, 2> capacity_right({{7, 8}, {9, 10}, {11, 12}});
    const auto fixed_product = fast_matrix_multiply(capacity_left, capacity_right);
    assert(fixed_product.rows() == 2);
    assert(fixed_product.cols() == 2);
    assert(fixed_product(0, 0) == 58);
    assert(fixed_product(0, 1) == 64);
    assert(fixed_product(1, 0) == 139);
    assert(fixed_product(1, 1) == 154);


    constexpr std::size_t recursive_size = 128;
    std::vector<ExactRing> large_identity(recursive_size * recursive_size);
    std::vector<ExactRing> large_matrix(recursive_size * recursive_size);
    for(std::size_t row = 0; row < recursive_size; ++row){
        large_identity[row * recursive_size + row] = ExactRing(1);
        for(std::size_t column = 0; column < recursive_size; ++column){
            large_matrix[row * recursive_size + column] = ExactRing(
                static_cast<long long>((row * 3 + column * 5) % 17) - 8
            );
        }
    }
    assert(fast_matrix_multiply(
        large_identity, recursive_size, recursive_size,
        large_matrix, recursive_size
    ) == large_matrix);

    bool thrown = false;
    try{
        (void)fast_matrix_multiply(
            std::vector<long long>{1, 2}, 2, 2,
            std::vector<long long>{1, 2, 3, 4}, 2
        );
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)fast_matrix_multiply(
            std::vector<long long>{1, 2, 3, 4}, 2, 2,
            std::vector<long long>{1, 2}, 2
        );
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        const std::size_t huge = std::numeric_limits<std::size_t>::max() / 2 + 1;
        (void)fast_matrix_multiply(
            std::vector<long long>{}, huge, 2,
            std::vector<long long>{}, 0
        );
    }catch(const std::length_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        Matrix<long long, 2, 2> fixed_left(1, 2);
        Matrix<long long, 2, 2> fixed_right(1, 1);
        (void)fast_matrix_multiply(fixed_left, fixed_right);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    self_test_contracts();

    int query_count = 0;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- > 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "INVALID"){
            std::cout << "OK\n";
            continue;
        }

        std::size_t rows = 0;
        std::size_t inner = 0;
        std::size_t columns = 0;
        if(operation == "SAME"){
            std::cin >> rows;
            inner = rows;
            columns = rows;
        }else{
            std::cin >> rows >> inner >> columns;
        }

        std::vector<long long> left(rows * inner);
        for(long long& value: left) std::cin >> value;
        std::vector<long long> right;
        if(operation == "SAME"){
            right = left;
        }else{
            right.resize(inner * columns);
            for(long long& value: right) std::cin >> value;
        }
        const std::vector<long long> saved_left = left;
        const std::vector<long long> saved_right = right;

        std::vector<long long> actual;
        if(operation == "SAME"){
            actual = fast_matrix_multiply(left, rows, inner, left, columns);
        }else if(operation == "MATRIX"){
            assert(rows <= 96 && inner <= 96 && columns <= 96);
            Matrix<long long, 96, 96> fixed_left(
                static_cast<int>(rows), static_cast<int>(inner)
            );
            Matrix<long long, 96, 96> fixed_right(
                static_cast<int>(inner), static_cast<int>(columns)
            );
            for(std::size_t row = 0; row < rows; ++row){
                for(std::size_t column = 0; column < inner; ++column){
                    fixed_left(static_cast<int>(row), static_cast<int>(column)) =
                        left[row * inner + column];
                }
            }
            for(std::size_t row = 0; row < inner; ++row){
                for(std::size_t column = 0; column < columns; ++column){
                    fixed_right(static_cast<int>(row), static_cast<int>(column)) =
                        right[row * columns + column];
                }
            }
            const auto fixed_output = fast_matrix_multiply(fixed_left, fixed_right);
            actual.resize(rows * columns);
            for(std::size_t row = 0; row < rows; ++row){
                for(std::size_t column = 0; column < columns; ++column){
                    actual[row * columns + column] = fixed_output(
                        static_cast<int>(row), static_cast<int>(column)
                    );
                }
            }
        }else{
            assert(operation == "FLAT");
            actual = fast_matrix_multiply(
                left, rows, inner, right, columns
            );
        }

        const auto expected = naive_matrix_multiply(
            left, rows, inner, right, columns
        );
        assert(actual == expected);
        assert(left == saved_left);
        assert(right == saved_right);
        print_matrix(rows, columns, actual);
    }
}
