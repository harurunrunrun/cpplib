#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

#include "min_plus_convolution.hpp"

namespace min_plus_convolution_concave_internal{

using index_type = std::ptrdiff_t;

template<std::signed_integral T>
bool concave(const std::vector<T>& values){
    for(std::size_t i = 1; i + 1 < values.size(); ++i){
        const min_plus_convolution_internal::ExactSum<T> outer(
            values[i - 1], values[i + 1]
        );
        const min_plus_convolution_internal::ExactSum<T> middle(
            values[i], values[i]
        );
        if(middle < outer) return false;
    }
    return true;
}

template<class Value, class Evaluate, class Report>
void monotone_minima(index_type row_left, index_type row_right,
                     index_type column_left, index_type column_right,
                     const Evaluate& evaluate, const Report& report){
    if(row_right < row_left) return;
    const index_type row = row_left + (row_right - row_left) / 2;
    index_type best_column = column_left;
    Value best = evaluate(row, best_column);
    for(index_type column = column_left + 1;
        column <= column_right; ++column){
        Value candidate = evaluate(row, column);
        if(candidate < best){
            best = candidate;
            best_column = column;
        }
    }
    report(row, best);
    monotone_minima<Value>(row_left, row - 1, column_left, best_column,
                           evaluate, report);
    monotone_minima<Value>(row + 1, row_right, best_column, column_right,
                           evaluate, report);
}

template<std::signed_integral T>
std::vector<T> convolution(const std::vector<T>& arbitrary_values,
                           const std::vector<T>& concave_values){
    if(arbitrary_values.empty() || concave_values.empty()) return {};

    constexpr std::size_t index_limit =
        static_cast<std::size_t>(std::numeric_limits<index_type>::max());
    if(index_limit < arbitrary_values.size() ||
       index_limit < concave_values.size() ||
       index_limit - (arbitrary_values.size() - 1) < concave_values.size()){
        throw std::length_error("min-plus convolution input is too large");
    }

    using exact_sum = min_plus_convolution_internal::ExactSum<T>;
    const index_type arbitrary_size =
        static_cast<index_type>(arbitrary_values.size());
    const index_type concave_size =
        static_cast<index_type>(concave_values.size());
    const index_type output_size = arbitrary_size + concave_size - 1;
    std::vector<std::optional<exact_sum>> best(
        static_cast<std::size_t>(output_size)
    );

    const auto update = [&](index_type output, const exact_sum& value){
        auto& current = best[static_cast<std::size_t>(output)];
        if(!current || value < *current) current = value;
    };
    const auto minimum_arbitrary_index = [&](index_type output){
        return std::max<index_type>(0, output - concave_size + 1);
    };
    const auto maximum_arbitrary_index = [&](index_type output){
        return std::min(arbitrary_size - 1, output);
    };

    constexpr index_type brute_force_limit = 1024;
    std::function<void(index_type, index_type, index_type, index_type)> solve;
    solve = [&](index_type output_left, index_type output_right,
                index_type arbitrary_left, index_type arbitrary_right){
        if(output_right < output_left || arbitrary_right < arbitrary_left) return;

        if(maximum_arbitrary_index(output_left) >= arbitrary_right &&
           arbitrary_left >= minimum_arbitrary_index(output_right)){
            const index_type row_count = output_right - output_left + 1;
            const index_type column_count = arbitrary_right - arbitrary_left + 1;
            const auto evaluate = [&](index_type row, index_type column){
                const index_type output = output_left + row;
                const index_type arbitrary_index = arbitrary_right - column;
                const index_type concave_index = output - arbitrary_index;
                return exact_sum(
                    arbitrary_values[static_cast<std::size_t>(arbitrary_index)],
                    concave_values[static_cast<std::size_t>(concave_index)]
                );
            };
            const auto report = [&](index_type row, const exact_sum& value){
                update(output_left + row, value);
            };
            monotone_minima<exact_sum>(
                0, row_count - 1, 0, column_count - 1, evaluate, report
            );
            return;
        }

        const index_type row_count = output_right - output_left + 1;
        const index_type column_count = arbitrary_right - arbitrary_left + 1;
        if(row_count <= brute_force_limit &&
           column_count <= brute_force_limit / row_count){
            for(index_type output = output_left;
                output <= output_right; ++output){
                const index_type from = std::max(
                    arbitrary_left, minimum_arbitrary_index(output)
                );
                const index_type to = std::min(
                    arbitrary_right, maximum_arbitrary_index(output)
                );
                for(index_type arbitrary_index = from;
                    arbitrary_index <= to; ++arbitrary_index){
                    update(output, exact_sum(
                        arbitrary_values[static_cast<std::size_t>(arbitrary_index)],
                        concave_values[static_cast<std::size_t>(
                            output - arbitrary_index
                        )]
                    ));
                }
            }
            return;
        }

        if(output_right - output_left > arbitrary_right - arbitrary_left){
            const index_type middle =
                output_left + (output_right - output_left) / 2;
            const index_type first_arbitrary_right = std::min(
                maximum_arbitrary_index(middle), arbitrary_right
            );
            if(arbitrary_left <= first_arbitrary_right){
                solve(output_left, middle, arbitrary_left,
                      first_arbitrary_right);
            }
            const index_type second_arbitrary_left = std::max(
                minimum_arbitrary_index(middle), arbitrary_left
            );
            if(second_arbitrary_left <= arbitrary_right){
                solve(middle + 1, output_right, second_arbitrary_left,
                      arbitrary_right);
            }
        }else{
            const index_type middle =
                arbitrary_left + (arbitrary_right - arbitrary_left) / 2;
            const index_type first_output_right = std::min(
                middle + concave_size - 1, output_right
            );
            if(output_left <= first_output_right){
                solve(output_left, first_output_right, arbitrary_left, middle);
            }
            const index_type second_output_left = std::max(middle, output_left);
            if(second_output_left <= output_right){
                solve(second_output_left, output_right, middle + 1,
                      arbitrary_right);
            }
        }
    };

    solve(0, output_size - 1, 0, arbitrary_size - 1);
    std::vector<T> result(static_cast<std::size_t>(output_size));
    for(index_type output = 0; output < output_size; ++output){
        const auto& value = best[static_cast<std::size_t>(output)];
        if(!value) throw std::logic_error("internal concave convolution error");
        result[static_cast<std::size_t>(output)] = value->checked_value();
    }
    return result;
}

} // namespace min_plus_convolution_concave_internal

template<std::signed_integral T>
bool is_discrete_concave(const std::vector<T>& values){
    return min_plus_convolution_concave_internal::concave(values);
}

template<std::signed_integral T>
std::vector<T> min_plus_convolution_concave_arbitrary(
    const std::vector<T>& concave_values,
    const std::vector<T>& arbitrary_values
){
    if(concave_values.empty() || arbitrary_values.empty()) return {};
    if(!is_discrete_concave(concave_values))[[unlikely]]{
        throw std::invalid_argument(
            "min_plus_convolution_concave_arbitrary requires a concave first sequence"
        );
    }
    return min_plus_convolution_concave_internal::convolution(
        arbitrary_values, concave_values
    );
}

template<std::signed_integral T>
std::vector<T> min_plus_convolution_arbitrary_concave(
    const std::vector<T>& arbitrary_values,
    const std::vector<T>& concave_values
){
    if(arbitrary_values.empty() || concave_values.empty()) return {};
    if(!is_discrete_concave(concave_values))[[unlikely]]{
        throw std::invalid_argument(
            "min_plus_convolution_arbitrary_concave requires a concave second sequence"
        );
    }
    return min_plus_convolution_concave_internal::convolution(
        arbitrary_values, concave_values
    );
}
