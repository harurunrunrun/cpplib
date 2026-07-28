#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_INTERVAL_PARTITION_MONGE_DP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_INTERVAL_PARTITION_MONGE_DP_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <vector>

#include "../../utility/smawk.hpp"
#include "divide_and_conquer_dp_optimization.hpp"

template<class Value, class Cost>
DivideAndConquerDPResult<Value> monge_dp_layer(
    const std::vector<Value>& previous,
    Cost cost,
    Value infinity
){
    if(previous.empty())[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: empty previous layer "
            "(monge_dp_layer)."
        );
    }
    const int size = static_cast<int>(previous.size()) - 1;
    DivideAndConquerDPResult<Value> result{
        std::vector<Value>(previous.size(), infinity),
        std::vector<int>(previous.size(), -1)
    };
    if(size == 0) return result;
    const auto matrix_value = [&](int row, int column){
        const int end = row + 1;
        if(column >= end) return infinity;
        return previous[static_cast<std::size_t>(column)]
            + std::invoke(cost, column, end);
    };
    std::vector<int> minima = smawk_row_minima(size, size, matrix_value);
    for(int row = 0; row < size; ++row){
        const int end = row + 1;
        const int split = minima[static_cast<std::size_t>(row)];
        result.optimum[static_cast<std::size_t>(end)] = split;
        result.value[static_cast<std::size_t>(end)] =
            matrix_value(row, split);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_INTERVAL_PARTITION_MONGE_DP_HPP_INCLUDED
