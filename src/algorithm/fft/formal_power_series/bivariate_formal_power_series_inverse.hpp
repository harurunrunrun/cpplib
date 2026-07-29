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
    using Mint = Modint<MOD>;
    const auto validate_length = [](std::size_t length){
        if(!convolution_internal::is_power_of_two(length)
            || length > static_cast<std::size_t>(
                std::numeric_limits<int>::max()
            )
            || static_cast<std::size_t>(MOD - 1) % length != 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid bivariate transform size"
            );
        }
    };
    validate_length(row_count);
    validate_length(column_count);
    const Mint root{convolution_internal::primitive_root<MOD>()};

    if(column_count > 1){
        for(std::size_t row = 0; row < row_count; ++row){
            const std::size_t base = row * column_count;
            for(std::size_t index = 1, reverse = 0;
                index < column_count; ++index){
                std::size_t bit = column_count >> 1;
                while((reverse & bit) != 0){
                    reverse ^= bit;
                    bit >>= 1;
                }
                reverse ^= bit;
                if(index < reverse){
                    std::swap(
                        values[base + index], values[base + reverse]
                    );
                }
            }
        }
        for(std::size_t length = 2; length <= column_count;
            length <<= 1){
            Mint root_step = root.pow(
                static_cast<long long>((MOD - 1) / length)
            );
            if(inverse) root_step = root_step.inv();
            const std::size_t half = length >> 1;
            for(std::size_t row = 0; row < row_count; ++row){
                const std::size_t row_base = row * column_count;
                for(std::size_t block = 0; block < column_count;
                    block += length){
                    Mint factor{1};
                    for(std::size_t offset = 0; offset < half; ++offset){
                        const std::size_t first =
                            row_base + block + offset;
                        const std::size_t second = first + half;
                        const Mint left = values[first];
                        const Mint right = values[second] * factor;
                        values[first] = left + right;
                        values[second] = left - right;
                        factor *= root_step;
                    }
                }
            }
        }
        if(inverse){
            const Mint inverse_columns{
                static_cast<long long>(column_count)
            };
            const Mint scale = inverse_columns.inv();
            for(Mint& value: values) value *= scale;
        }
    }

    if(row_count <= 1) return;
    for(std::size_t index = 1, reverse = 0; index < row_count; ++index){
        std::size_t bit = row_count >> 1;
        while((reverse & bit) != 0){
            reverse ^= bit;
            bit >>= 1;
        }
        reverse ^= bit;
        if(index < reverse){
            for(std::size_t column = 0; column < column_count; ++column){
                std::swap(
                    values[index * column_count + column],
                    values[reverse * column_count + column]
                );
            }
        }
    }
    for(std::size_t length = 2; length <= row_count; length <<= 1){
        Mint root_step = root.pow(
            static_cast<long long>((MOD - 1) / length)
        );
        if(inverse) root_step = root_step.inv();
        const std::size_t half = length >> 1;
        for(std::size_t block = 0; block < row_count; block += length){
            Mint factor{1};
            for(std::size_t offset = 0; offset < half; ++offset){
                const std::size_t first =
                    (block + offset) * column_count;
                const std::size_t second = first + half * column_count;
                for(std::size_t column = 0; column < column_count; ++column){
                    const Mint left = values[first + column];
                    const Mint right = values[second + column] * factor;
                    values[first + column] = left + right;
                    values[second + column] = left - right;
                }
                factor *= root_step;
            }
        }
    }
    if(inverse){
        const Mint inverse_rows{static_cast<long long>(row_count)};
        const Mint scale = inverse_rows.inv();
        for(Mint& value: values) value *= scale;
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
