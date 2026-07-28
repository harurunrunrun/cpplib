#ifndef CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_MONOTONE_MINIMA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_MONOTONE_MINIMA_HPP_INCLUDED

#include <concepts>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

template<class Value>
requires std::invocable<Value&, int, int>
std::vector<int> monotone_minima(
    int row_count,
    int column_count,
    Value value
){
    if(row_count < 0 || column_count < 0)[[unlikely]]{
        throw std::invalid_argument(
            "monotone_minima requires nonnegative dimensions"
        );
    }
    if(row_count != 0 && column_count == 0)[[unlikely]]{
        throw std::invalid_argument(
            "monotone_minima requires a nonempty column set"
        );
    }

    std::vector<int> result(static_cast<std::size_t>(row_count));
    const auto solve = [&](auto&& self, int first_row, int last_row,
                           int first_column, int last_column) -> void{
        if(first_row >= last_row) return;
        const int middle_row = first_row + (last_row - first_row) / 2;
        int best_column = first_column;
        auto best_value = value(middle_row, first_column);
        for(int column = first_column + 1; column < last_column; ++column){
            auto candidate = value(middle_row, column);
            if(candidate < best_value){
                best_value = std::move(candidate);
                best_column = column;
            }
        }
        result[static_cast<std::size_t>(middle_row)] = best_column;
        self(
            self,
            first_row,
            middle_row,
            first_column,
            best_column + 1
        );
        self(
            self,
            middle_row + 1,
            last_row,
            best_column,
            last_column
        );
    };
    if(row_count != 0){
        solve(solve, 0, row_count, 0, column_count);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_MONOTONE_MINIMA_HPP_INCLUDED
