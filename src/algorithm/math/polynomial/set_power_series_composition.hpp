#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_SET_POWER_SERIES_COMPOSITION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_SET_POWER_SERIES_COMPOSITION_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "subset_convolution.hpp"

namespace set_power_series_composition_detail{

template<class T>
void subset_zeta_transform(std::vector<T>& values){
    for(std::size_t block_size = 1;
        block_size < values.size();
        block_size <<= 1){
        for(std::size_t block = 0; block < values.size();
            block += block_size << 1){
            for(std::size_t offset = 0; offset < block_size; ++offset){
                values[block + block_size + offset] +=
                    values[block + offset];
            }
        }
    }
}

template<class T>
void subset_mobius_transform(std::vector<T>& values){
    for(std::size_t block_size = 1;
        block_size < values.size();
        block_size <<= 1){
        for(std::size_t block = 0; block < values.size();
            block += block_size << 1){
            for(std::size_t offset = 0; offset < block_size; ++offset){
                values[block + block_size + offset] -=
                    values[block + offset];
            }
        }
    }
}

inline std::size_t validate_series_size(std::size_t size){
    if(size == 0 || !std::has_single_bit(size))[[unlikely]]{
        throw std::invalid_argument(
            "set power series size must be a nonempty power of two"
        );
    }
    return std::bit_width(size) - 1;
}

template<class T>
std::vector<T> transposed_subset_product(
    const std::vector<T>& multiplier,
    std::vector<T> weights
){
    std::reverse(weights.begin(), weights.end());
    weights = subset_convolution(weights, multiplier);
    std::reverse(weights.begin(), weights.end());
    return weights;
}

template<class T>
std::vector<T> factorials(std::size_t maximum){
    std::vector<T> result(maximum + 1, T{1});
    for(std::size_t value = 1; value <= maximum; ++value){
        result[value] = result[value - 1]
            * T{static_cast<long long>(value)};
    }
    return result;
}

template<class T>
std::vector<T> inverse_factorials(const std::vector<T>& factorial){
    std::vector<T> result(factorial.size(), T{1});
    if(factorial.empty()) return result;
    result.back() = T{1} / factorial.back();
    for(std::size_t value = factorial.size() - 1; value > 0; --value){
        result[value - 1] = result[value]
            * T{static_cast<long long>(value)};
    }
    return result;
}

} // namespace set_power_series_composition_detail

template<class T>
std::vector<T> compose_exponential_generating_function_set_power_series(
    const std::vector<T>& exponential_coefficients,
    const std::vector<T>& series
){
    using namespace set_power_series_composition_detail;
    const std::size_t bit_count = validate_series_size(series.size());
    if(exponential_coefficients.size() != bit_count + 1)[[unlikely]]{
        throw std::invalid_argument(
            "exponential coefficient count must equal bit count plus one"
        );
    }
    if(!(series[0] == T{}))[[unlikely]]{
        throw std::domain_error(
            "EGF composition requires zero empty-set coefficient"
        );
    }
    if(bit_count >= std::numeric_limits<std::size_t>::digits)[[unlikely]]{
        throw std::length_error("set power series bit count is too large");
    }

    std::vector<std::vector<std::vector<T>>> levels(bit_count + 1);
    for(std::size_t level = 0; level <= bit_count; ++level){
        const std::size_t subset_count = std::size_t{1} << level;
        levels[level].assign(
            level + 1,
            std::vector<T>(subset_count)
        );
        levels[level][0][0] =
            exponential_coefficients[bit_count - level];
    }

    for(std::size_t dimension = 0;
        dimension < bit_count;
        ++dimension){
        const std::size_t half_size = std::size_t{1} << dimension;
        std::vector<std::vector<T>> transformed_slice(
            dimension + 1,
            std::vector<T>(half_size)
        );
        for(std::size_t subset = 0; subset < half_size; ++subset){
            transformed_slice[std::popcount(subset)][subset] =
                series[half_size + subset];
        }
        for(auto& rank_values: transformed_slice){
            subset_zeta_transform(rank_values);
        }

        for(std::size_t level = dimension + 1;
            level <= bit_count;
            ++level){
            for(auto& rank_values: levels[level]){
                std::copy_n(
                    rank_values.begin(),
                    static_cast<std::ptrdiff_t>(half_size),
                    rank_values.begin()
                        + static_cast<std::ptrdiff_t>(half_size)
                );
            }
        }

        for(std::size_t level = dimension;
            level < bit_count;
            ++level){
            for(std::size_t slice_rank = 0;
                slice_rank <= dimension;
                ++slice_rank){
                for(std::size_t previous_rank = 0;
                    slice_rank + previous_rank <= level;
                    ++previous_rank){
                    const auto& left = transformed_slice[slice_rank];
                    const auto& right = levels[level][previous_rank];
                    auto& destination =
                        levels[level + 1][
                            slice_rank + previous_rank + 1
                        ];
                    for(std::size_t subset = 0;
                        subset < half_size;
                        ++subset){
                        destination[half_size + subset] +=
                            left[subset] * right[subset];
                    }
                }
            }
        }
    }

    for(auto& rank_values: levels[bit_count]){
        subset_mobius_transform(rank_values);
    }
    std::vector<T> result(series.size());
    for(std::size_t subset = 0; subset < series.size(); ++subset){
        result[subset] =
            levels[bit_count][std::popcount(subset)][subset];
    }
    return result;
}

template<class T>
std::vector<T> polynomial_composite_set_power_series(
    const std::vector<T>& polynomial,
    const std::vector<T>& series
){
    using namespace set_power_series_composition_detail;
    const std::size_t bit_count = validate_series_size(series.size());
    const T constant = series[0];
    std::vector<T> shifted_coefficients(bit_count + 1);
    for(auto iterator = polynomial.rbegin();
        iterator != polynomial.rend();
        ++iterator){
        for(std::size_t degree = bit_count; degree > 0; --degree){
            shifted_coefficients[degree] =
                shifted_coefficients[degree] * constant
                + shifted_coefficients[degree - 1];
        }
        shifted_coefficients[0] =
            shifted_coefficients[0] * constant + *iterator;
    }
    const std::vector<T> factorial = factorials<T>(bit_count);
    for(std::size_t degree = 0; degree <= bit_count; ++degree){
        shifted_coefficients[degree] *= factorial[degree];
    }
    std::vector<T> zero_constant_series = series;
    zero_constant_series[0] = T{};
    return compose_exponential_generating_function_set_power_series(
        shifted_coefficients,
        zero_constant_series
    );
}

template<class T>
std::vector<T> set_power_series_power_projection(
    const std::vector<T>& series,
    const std::vector<T>& weights,
    std::size_t output_count
){
    using namespace set_power_series_composition_detail;
    const std::size_t bit_count = validate_series_size(series.size());
    if(weights.size() != series.size())[[unlikely]]{
        throw std::invalid_argument(
            "set power series and weight arrays must have equal sizes"
        );
    }
    if(output_count == 0) return {};

    const T constant = series[0];
    std::vector<T> zero_constant_series = series;
    zero_constant_series[0] = T{};
    std::vector<T> current_weights = weights;
    std::vector<T> exponential_projection(bit_count + 1);
    exponential_projection[0] = current_weights[0];

    for(std::size_t step = 0; step < bit_count; ++step){
        const std::size_t current_bit_count = bit_count - step;
        const std::size_t next_size =
            std::size_t{1} << (current_bit_count - 1);
        std::vector<T> next_weights(next_size);
        for(std::size_t dimension = 0;
            dimension < current_bit_count;
            ++dimension){
            const std::size_t block_size = std::size_t{1} << dimension;
            std::vector<T> multiplier(
                zero_constant_series.begin()
                    + static_cast<std::ptrdiff_t>(block_size),
                zero_constant_series.begin()
                    + static_cast<std::ptrdiff_t>(block_size << 1)
            );
            std::vector<T> block_weights(
                current_weights.begin()
                    + static_cast<std::ptrdiff_t>(block_size),
                current_weights.begin()
                    + static_cast<std::ptrdiff_t>(block_size << 1)
            );
            const std::vector<T> contribution =
                transposed_subset_product(
                    multiplier,
                    std::move(block_weights)
                );
            for(std::size_t index = 0;
                index < contribution.size();
                ++index){
                next_weights[index] += contribution[index];
            }
        }
        current_weights = std::move(next_weights);
        exponential_projection[step + 1] = current_weights[0];
    }

    const std::size_t factorial_limit =
        std::max(bit_count, output_count - 1);
    const std::vector<T> factorial = factorials<T>(factorial_limit);
    const std::vector<T> inverse_factorial =
        inverse_factorials(factorial);
    std::vector<T> constant_kernel(output_count);
    T constant_power{1};
    for(std::size_t degree = 0; degree < output_count; ++degree){
        constant_kernel[degree] =
            constant_power * inverse_factorial[degree];
        constant_power *= constant;
    }

    std::vector<T> result(output_count);
    for(std::size_t degree = 0; degree < output_count; ++degree){
        T value{};
        const std::size_t last =
            std::min(bit_count, degree);
        for(std::size_t used = 0; used <= last; ++used){
            value += exponential_projection[used]
                * constant_kernel[degree - used];
        }
        result[degree] = value * factorial[degree];
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_SET_POWER_SERIES_COMPOSITION_HPP_INCLUDED
