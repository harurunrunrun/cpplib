#ifndef CPPLIB_SRC_ALGORITHM_MATH_HAFNIAN_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_HAFNIAN_HPP_INCLUDED

#include <bit>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
#include "subset_convolution.hpp"
#include "../../structure/matrix/matrix.hpp"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

namespace hafnian_detail{

template<class T>
void validate(const std::vector<std::vector<T>>& matrix){
    const std::size_t size = matrix.size();
    if((size & 1U) != 0U)[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: odd size (hafnian)."
        );
    }
    for(const auto& row: matrix){
        if(row.size() != size)[[unlikely]]{
            throw std::invalid_argument(
                "library assertion fault: square violation (hafnian)."
            );
        }
    }
    for(std::size_t row = 0; row < size; ++row){
        for(std::size_t column = row + 1; column < size; ++column){
            if(matrix[row][column] != matrix[column][row])[[unlikely]]{
                throw std::invalid_argument(
                    "library assertion fault: symmetry violation (hafnian)."
                );
            }
        }
    }
}

template<class T>
std::vector<T> set_power_series_exp(const std::vector<T>& connected){
    const std::size_t subset_count = connected.size();
    const std::size_t bit_count = static_cast<std::size_t>(
        std::bit_width(subset_count) - 1
    );
    std::vector<T> result(subset_count);
    result[0] = T(1);
    for(std::size_t bit = 0; bit < bit_count; ++bit){
        const std::size_t half = std::size_t{1} << bit;
        std::vector<T> block(
            connected.begin() + static_cast<std::ptrdiff_t>(half),
            connected.begin() + static_cast<std::ptrdiff_t>(2 * half)
        );
        const std::vector<T> preceding(
            result.begin(),
            result.begin() + static_cast<std::ptrdiff_t>(half)
        );
        block = subset_convolution(block, preceding);
        for(std::size_t subset = 0; subset < half; ++subset){
            result[half + subset] = std::move(block[subset]);
        }
    }
    return result;
}

}  // namespace hafnian_detail

template<class T>
T hafnian(const std::vector<std::vector<T>>& matrix){
    hafnian_detail::validate(matrix);
    const std::size_t size = matrix.size();
    const std::size_t half_size = size / 2;
    if(half_size >= std::numeric_limits<std::size_t>::digits)[[unlikely]]{
        throw std::length_error("library assertion fault: size overflow (hafnian).");
    }
    const std::size_t subset_count = std::size_t{1} << half_size;
    std::vector<T> connected(subset_count);

    for(std::size_t last_pair = 0; last_pair < half_size; ++last_pair){
        const std::size_t first_vertex = 2 * last_pair;
        const std::size_t second_vertex = first_vertex + 1;
        const std::size_t last_flag = std::size_t{1} << last_pair;
        connected[last_flag] += matrix[first_vertex][second_vertex];

        const std::size_t stride = 2 * last_pair;
        if(stride == 0) continue;
        std::vector<T> paths(stride * last_flag);
        for(std::size_t pair = 0; pair < last_pair; ++pair){
            const std::size_t even = 2 * pair;
            const std::size_t odd = even + 1;
            const std::size_t offset = stride * (std::size_t{1} << pair);
            paths[offset + even] += matrix[first_vertex][odd];
            paths[offset + odd] += matrix[first_vertex][even];
        }

        for(std::size_t subset = 1; subset < last_flag; ++subset){
            const std::size_t offset = stride * subset;
            std::size_t endpoint_pairs = subset;
            while(endpoint_pairs != 0){
                const std::size_t pair = static_cast<std::size_t>(
                    std::countr_zero(endpoint_pairs)
                );
                endpoint_pairs &= endpoint_pairs - 1;
                const std::size_t even = 2 * pair;
                const std::size_t odd = even + 1;
                const T even_paths = paths[offset + even];
                const T odd_paths = paths[offset + odd];
                if(even_paths == T{} && odd_paths == T{}) continue;

                connected[subset | last_flag] +=
                    even_paths * matrix[second_vertex][even] +
                    odd_paths * matrix[second_vertex][odd];

                std::size_t unused_pairs = (last_flag - 1) ^ subset;
                while(unused_pairs != 0){
                    const std::size_t next_pair = static_cast<std::size_t>(
                        std::countr_zero(unused_pairs)
                    );
                    unused_pairs &= unused_pairs - 1;
                    const std::size_t next_even = 2 * next_pair;
                    const std::size_t next_odd = next_even + 1;
                    const std::size_t next_offset = stride * (
                        subset | (std::size_t{1} << next_pair)
                    );
                    paths[next_offset + next_even] +=
                        even_paths * matrix[even][next_odd] +
                        odd_paths * matrix[odd][next_odd];
                    paths[next_offset + next_odd] +=
                        even_paths * matrix[even][next_even] +
                        odd_paths * matrix[odd][next_even];
                }
            }
        }
    }

    return hafnian_detail::set_power_series_exp(connected).back();
}

template<class T, int MAX_ROW, int MAX_COL>
T hafnian(const Matrix<T, MAX_ROW, MAX_COL>& matrix){
    if(matrix.rows() != matrix.cols())[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: square violation (hafnian)."
        );
    }
    std::vector<std::vector<T>> values(
        static_cast<std::size_t>(matrix.rows()),
        std::vector<T>(static_cast<std::size_t>(matrix.cols()))
    );
    for(int row = 0; row < matrix.rows(); ++row){
        for(int column = 0; column < matrix.cols(); ++column){
            values[static_cast<std::size_t>(row)]
                [static_cast<std::size_t>(column)] = matrix(row, column);
        }
    }
    return hafnian(values);
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_HAFNIAN_HPP_INCLUDED
