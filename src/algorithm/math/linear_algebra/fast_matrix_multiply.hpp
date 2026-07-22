#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_MATRIX_MULTIPLY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_MATRIX_MULTIPLY_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../../structure/matrix/matrix.hpp"

namespace fast_matrix_multiply_internal{
inline constexpr std::size_t naive_threshold = 64;


inline std::size_t checked_area(std::size_t rows, std::size_t columns){
    if(rows != 0 && columns > std::numeric_limits<std::size_t>::max() / rows){
        throw std::length_error(
            "library assertion fault: size overflow (fast_matrix_multiply)."
        );
    }
    return rows * columns;
}

inline std::size_t padded_size(std::size_t size){
    if(size == 0) return 0;
    std::size_t result = 1;
    while(result < size){
        if(result > std::numeric_limits<std::size_t>::max() / 2){
            throw std::length_error(
                "library assertion fault: size overflow (fast_matrix_multiply)."
            );
        }
        result *= 2;
    }
    (void)checked_area(result, result);
    return result;
}
inline std::size_t saturating_product(
    std::size_t left,
    std::size_t right
){
    if(left != 0 && right > std::numeric_limits<std::size_t>::max() / left){
        return std::numeric_limits<std::size_t>::max();
    }
    return left * right;
}

inline std::size_t strassen_scalar_product_budget(std::size_t size){
    if(size <= naive_threshold){
        return saturating_product(saturating_product(size, size), size);
    }
    return saturating_product(
        7, strassen_scalar_product_budget(size / 2)
    );
}

template<class T>
void multiply_rectangular_naive(
    const std::vector<T>& left,
    std::size_t left_rows,
    std::size_t inner_size,
    const std::vector<T>& right,
    std::size_t right_columns,
    std::vector<T>& output
){
    for(std::size_t row = 0; row < left_rows; ++row){
        for(std::size_t middle = 0; middle < inner_size; ++middle){
            const T& value = left[row * inner_size + middle];
            for(std::size_t column = 0; column < right_columns; ++column){
                output[row * right_columns + column] +=
                    value * right[middle * right_columns + column];
            }
        }
    }
}


template<class T>
void assign_sum_or_difference(
    T* output,
    const T* left,
    std::size_t left_stride,
    const T* right,
    std::size_t right_stride,
    std::size_t size,
    bool subtract
){
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            if(subtract){
                output[row * size + column] =
                    left[row * left_stride + column] -
                    right[row * right_stride + column];
            }else{
                output[row * size + column] =
                    left[row * left_stride + column] +
                    right[row * right_stride + column];
            }
        }
    }
}

template<class T>
void assign_copy(
    T* output,
    const T* input,
    std::size_t input_stride,
    std::size_t size
){
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            output[row * size + column] = input[row * input_stride + column];
        }
    }
}

template<class T>
void accumulate(
    T* output,
    std::size_t output_stride,
    const T* input,
    std::size_t size,
    bool subtract
){
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            if(subtract){
                output[row * output_stride + column] -= input[row * size + column];
            }else{
                output[row * output_stride + column] += input[row * size + column];
            }
        }
    }
}

template<class T>
void multiply_square(
    const T* left,
    std::size_t left_stride,
    const T* right,
    std::size_t right_stride,
    T* output,
    std::size_t output_stride,
    std::size_t size,
    T* workspace
){
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = 0; column < size; ++column){
            output[row * output_stride + column] = T{};
        }
    }
    if(size <= naive_threshold){
        for(std::size_t row = 0; row < size; ++row){
            for(std::size_t middle = 0; middle < size; ++middle){
                const T& value = left[row * left_stride + middle];
                for(std::size_t column = 0; column < size; ++column){
                    output[row * output_stride + column] +=
                        value * right[middle * right_stride + column];
                }
            }
        }
        return;
    }

    const std::size_t half = size / 2;
    const std::size_t block_area = half * half;
    T* temporary_left = workspace;
    T* temporary_right = temporary_left + block_area;
    T* product = temporary_right + block_area;
    T* child_workspace = product + block_area;

    const T* a11 = left;
    const T* a12 = left + half;
    const T* a21 = left + half * left_stride;
    const T* a22 = a21 + half;
    const T* b11 = right;
    const T* b12 = right + half;
    const T* b21 = right + half * right_stride;
    const T* b22 = b21 + half;
    T* c11 = output;
    T* c12 = output + half;
    T* c21 = output + half * output_stride;
    T* c22 = c21 + half;

    assign_sum_or_difference(
        temporary_left, a11, left_stride, a22, left_stride, half, false
    );
    assign_sum_or_difference(
        temporary_right, b11, right_stride, b22, right_stride, half, false
    );
    multiply_square(
        temporary_left, half, temporary_right, half, product, half, half,
        child_workspace
    );
    accumulate(c11, output_stride, product, half, false);
    accumulate(c22, output_stride, product, half, false);

    assign_sum_or_difference(
        temporary_left, a21, left_stride, a22, left_stride, half, false
    );
    assign_copy(temporary_right, b11, right_stride, half);
    multiply_square(
        temporary_left, half, temporary_right, half, product, half, half,
        child_workspace
    );
    accumulate(c21, output_stride, product, half, false);
    accumulate(c22, output_stride, product, half, true);

    assign_copy(temporary_left, a11, left_stride, half);
    assign_sum_or_difference(
        temporary_right, b12, right_stride, b22, right_stride, half, true
    );
    multiply_square(
        temporary_left, half, temporary_right, half, product, half, half,
        child_workspace
    );
    accumulate(c12, output_stride, product, half, false);
    accumulate(c22, output_stride, product, half, false);

    assign_copy(temporary_left, a22, left_stride, half);
    assign_sum_or_difference(
        temporary_right, b21, right_stride, b11, right_stride, half, true
    );
    multiply_square(
        temporary_left, half, temporary_right, half, product, half, half,
        child_workspace
    );
    accumulate(c11, output_stride, product, half, false);
    accumulate(c21, output_stride, product, half, false);

    assign_sum_or_difference(
        temporary_left, a11, left_stride, a12, left_stride, half, false
    );
    assign_copy(temporary_right, b22, right_stride, half);
    multiply_square(
        temporary_left, half, temporary_right, half, product, half, half,
        child_workspace
    );
    accumulate(c11, output_stride, product, half, true);
    accumulate(c12, output_stride, product, half, false);

    assign_sum_or_difference(
        temporary_left, a21, left_stride, a11, left_stride, half, true
    );
    assign_sum_or_difference(
        temporary_right, b11, right_stride, b12, right_stride, half, false
    );
    multiply_square(
        temporary_left, half, temporary_right, half, product, half, half,
        child_workspace
    );
    accumulate(c22, output_stride, product, half, false);

    assign_sum_or_difference(
        temporary_left, a12, left_stride, a22, left_stride, half, true
    );
    assign_sum_or_difference(
        temporary_right, b21, right_stride, b22, right_stride, half, false
    );
    multiply_square(
        temporary_left, half, temporary_right, half, product, half, half,
        child_workspace
    );
    accumulate(c11, output_stride, product, half, false);
}

}  // namespace fast_matrix_multiply_internal

template<class T>
std::vector<T> fast_matrix_multiply(
    const std::vector<T>& left,
    std::size_t left_rows,
    std::size_t inner_size,
    const std::vector<T>& right,
    std::size_t right_columns
){
    using namespace fast_matrix_multiply_internal;
    const std::size_t left_area = checked_area(left_rows, inner_size);
    const std::size_t right_area = checked_area(inner_size, right_columns);
    if(left.size() != left_area || right.size() != right_area)[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: shape violation (fast_matrix_multiply)."
        );
    }
    const std::size_t output_area = checked_area(left_rows, right_columns);
    std::vector<T> output(output_area);
    if(left_rows == 0 || inner_size == 0 || right_columns == 0){
        return output;
    }

    const std::size_t size = padded_size(
        std::max({left_rows, inner_size, right_columns})
    );
    const std::size_t naive_products = saturating_product(
        saturating_product(left_rows, inner_size), right_columns
    );
    if(naive_products <= strassen_scalar_product_budget(size)){
        multiply_rectangular_naive(
            left, left_rows, inner_size, right, right_columns, output
        );
        return output;
    }
    const std::size_t square_area = checked_area(size, size);
    std::vector<T> padded_left(square_area);
    std::vector<T> padded_right(square_area);
    std::vector<T> padded_output(square_area);
    std::vector<T> workspace(square_area);
    for(std::size_t row = 0; row < left_rows; ++row){
        std::copy_n(
            left.data() + row * inner_size,
            inner_size,
            padded_left.data() + row * size
        );
    }
    for(std::size_t row = 0; row < inner_size; ++row){
        std::copy_n(
            right.data() + row * right_columns,
            right_columns,
            padded_right.data() + row * size
        );
    }
    multiply_square(
        padded_left.data(), size, padded_right.data(), size,
        padded_output.data(), size, size, workspace.data()
    );

    for(std::size_t row = 0; row < left_rows; ++row){
        std::copy_n(
            padded_output.data() + row * size,
            right_columns,
            output.data() + row * right_columns
        );
    }
    return output;
}

template<class T, int LEFT_MAX_ROW, int LEFT_MAX_COL, int RIGHT_MAX_ROW,
    int RIGHT_MAX_COL>
Matrix<T, LEFT_MAX_ROW, RIGHT_MAX_COL> fast_matrix_multiply(
    const Matrix<T, LEFT_MAX_ROW, LEFT_MAX_COL>& left,
    const Matrix<T, RIGHT_MAX_ROW, RIGHT_MAX_COL>& right
){
    if(left.cols() != right.rows())[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: shape violation (fast_matrix_multiply)."
        );
    }
    const std::size_t left_rows = static_cast<std::size_t>(left.rows());
    const std::size_t inner_size = static_cast<std::size_t>(left.cols());
    const std::size_t right_columns = static_cast<std::size_t>(right.cols());
    std::vector<T> left_values(left_rows * inner_size);
    std::vector<T> right_values(inner_size * right_columns);
    for(std::size_t row = 0; row < left_rows; ++row){
        for(std::size_t column = 0; column < inner_size; ++column){
            left_values[row * inner_size + column] = left(
                static_cast<int>(row), static_cast<int>(column)
            );
        }
    }
    for(std::size_t row = 0; row < inner_size; ++row){
        for(std::size_t column = 0; column < right_columns; ++column){
            right_values[row * right_columns + column] = right(
                static_cast<int>(row), static_cast<int>(column)
            );
        }
    }
    const std::vector<T> values = fast_matrix_multiply(
        left_values, left_rows, inner_size, right_values, right_columns
    );
    Matrix<T, LEFT_MAX_ROW, RIGHT_MAX_COL> output(
        left.rows(), right.cols()
    );
    for(std::size_t row = 0; row < left_rows; ++row){
        for(std::size_t column = 0; column < right_columns; ++column){
            output(static_cast<int>(row), static_cast<int>(column)) =
                values[row * right_columns + column];
        }
    }
    return output;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_FAST_MATRIX_MULTIPLY_HPP_INCLUDED
