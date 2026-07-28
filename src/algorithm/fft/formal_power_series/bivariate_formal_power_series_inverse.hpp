#ifndef CPPLIB_SRC_ALGORITHM_FFT_FORMAL_POWER_SERIES_BIVARIATE_FORMAL_POWER_SERIES_INVERSE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_FORMAL_POWER_SERIES_BIVARIATE_FORMAL_POWER_SERIES_INVERSE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../transform/convolution.hpp"

namespace bivariate_formal_power_series_inverse_detail{

template<int MOD>
using Matrix = std::vector<std::vector<Modint<MOD>>>;

inline std::size_t transform_size(std::size_t required){
    std::size_t result = 1;
    while(result < required){
        if(result > std::numeric_limits<std::size_t>::max() / 2)[[unlikely]]{
            throw std::length_error("bivariate transform size overflow");
        }
        result *= 2;
    }
    return result;
}

template<int MOD>
void transform_2d(
    std::vector<Modint<MOD>>& values,
    std::size_t row_count,
    std::size_t column_count,
    bool inverse
){
    std::vector<Modint<MOD>> buffer(
        std::max(row_count, column_count)
    );
    for(std::size_t row = 0; row < row_count; ++row){
        std::copy_n(
            values.begin() + static_cast<std::ptrdiff_t>(row * column_count),
            column_count,
            buffer.begin()
        );
        buffer.resize(column_count);
        number_theoretic_transform(buffer, inverse);
        std::copy(
            buffer.begin(), buffer.end(),
            values.begin() + static_cast<std::ptrdiff_t>(row * column_count)
        );
        buffer.resize(std::max(row_count, column_count));
    }
    for(std::size_t column = 0; column < column_count; ++column){
        for(std::size_t row = 0; row < row_count; ++row){
            buffer[row] = values[row * column_count + column];
        }
        buffer.resize(row_count);
        number_theoretic_transform(buffer, inverse);
        for(std::size_t row = 0; row < row_count; ++row){
            values[row * column_count + column] = buffer[row];
        }
        buffer.resize(std::max(row_count, column_count));
    }
}

template<int MOD>
Matrix<MOD> truncated_product(
    const Matrix<MOD>& left,
    const Matrix<MOD>& right,
    std::size_t result_rows,
    std::size_t result_columns
){
    const std::size_t full_rows =
        left.size() + right.size() - 1;
    const std::size_t full_columns =
        left.front().size() + right.front().size() - 1;
    const std::size_t transform_rows = transform_size(full_rows);
    const std::size_t transform_columns = transform_size(full_columns);
    if(transform_rows > std::numeric_limits<std::size_t>::max()
        / transform_columns)[[unlikely]]{
        throw std::length_error("bivariate transform storage overflow");
    }
    const std::size_t transform_count =
        transform_rows * transform_columns;
    std::vector<Modint<MOD>> left_values(transform_count);
    std::vector<Modint<MOD>> right_values(transform_count);
    for(std::size_t row = 0; row < left.size(); ++row){
        std::copy(
            left[row].begin(), left[row].end(),
            left_values.begin()
                + static_cast<std::ptrdiff_t>(row * transform_columns)
        );
    }
    for(std::size_t row = 0; row < right.size(); ++row){
        std::copy(
            right[row].begin(), right[row].end(),
            right_values.begin()
                + static_cast<std::ptrdiff_t>(row * transform_columns)
        );
    }
    transform_2d(
        left_values, transform_rows, transform_columns, false
    );
    transform_2d(
        right_values, transform_rows, transform_columns, false
    );
    for(std::size_t index = 0; index < transform_count; ++index){
        left_values[index] *= right_values[index];
    }
    transform_2d(
        left_values, transform_rows, transform_columns, true
    );

    Matrix<MOD> result(
        result_rows,
        std::vector<Modint<MOD>>(result_columns)
    );
    for(std::size_t row = 0; row < result_rows; ++row){
        std::copy_n(
            left_values.begin()
                + static_cast<std::ptrdiff_t>(row * transform_columns),
            result_columns,
            result[row].begin()
        );
    }
    return result;
}

template<int MOD>
void validate(const Matrix<MOD>& series){
    if(series.empty() || series.front().empty())[[unlikely]]{
        throw std::invalid_argument(
            "bivariate formal power series must be nonempty"
        );
    }
    const std::size_t column_count = series.front().size();
    for(const auto& row: series){
        if(row.size() != column_count)[[unlikely]]{
            throw std::invalid_argument(
                "bivariate formal power series must be rectangular"
            );
        }
    }
    if(series[0][0] == Modint<MOD>{})[[unlikely]]{
        throw std::domain_error(
            "bivariate formal power series constant term is zero"
        );
    }
}

}  // namespace bivariate_formal_power_series_inverse_detail

template<int MOD>
std::vector<std::vector<Modint<MOD>>>
bivariate_formal_power_series_inverse(
    const std::vector<std::vector<Modint<MOD>>>& series
){
    using namespace bivariate_formal_power_series_inverse_detail;
    validate(series);
    const std::size_t row_count = series.size();
    const std::size_t column_count = series.front().size();
    Matrix<MOD> inverse{{series[0][0].inv()}};
    while(inverse.size() < row_count
        || inverse.front().size() < column_count){
        std::size_t next_rows = inverse.size();
        std::size_t next_columns = inverse.front().size();
        if(next_columns < column_count){
            next_columns = std::min(column_count, next_columns * 2);
        }else{
            next_rows = std::min(row_count, next_rows * 2);
        }
        Matrix<MOD> prefix(
            next_rows,
            std::vector<Modint<MOD>>(next_columns)
        );
        for(std::size_t row = 0; row < next_rows; ++row){
            std::copy_n(
                series[row].begin(), next_columns, prefix[row].begin()
            );
        }
        Matrix<MOD> correction = truncated_product(
            prefix, inverse, next_rows, next_columns
        );
        for(auto& row: correction){
            for(auto& coefficient: row) coefficient = -coefficient;
        }
        correction[0][0] += Modint<MOD>{2};
        inverse = truncated_product(
            inverse, correction, next_rows, next_columns
        );
    }
    return inverse;
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_FORMAL_POWER_SERIES_BIVARIATE_FORMAL_POWER_SERIES_INVERSE_HPP_INCLUDED
