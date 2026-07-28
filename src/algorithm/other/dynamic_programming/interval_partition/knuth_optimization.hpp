#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_INTERVAL_PARTITION_KNUTH_OPTIMIZATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_INTERVAL_PARTITION_KNUTH_OPTIMIZATION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <vector>

template<class Value>
struct KnuthOptimizationResult{
    std::vector<std::vector<Value>> value;
    std::vector<std::vector<int>> optimum;
};

template<class Value, class IntervalCost>
KnuthOptimizationResult<Value> knuth_optimization(
    int size,
    IntervalCost interval_cost,
    Value infinity
){
    if(size < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative size (knuth_optimization)."
        );
    }
    KnuthOptimizationResult<Value> result{
        std::vector<std::vector<Value>>(
            static_cast<std::size_t>(size + 1),
            std::vector<Value>(static_cast<std::size_t>(size + 1), infinity)
        ),
        std::vector<std::vector<int>>(
            static_cast<std::size_t>(size + 1),
            std::vector<int>(static_cast<std::size_t>(size + 1), -1)
        )
    };
    for(int index = 0; index <= size; ++index){
        result.value[static_cast<std::size_t>(index)]
            [static_cast<std::size_t>(index)] = Value(0);
        result.optimum[static_cast<std::size_t>(index)]
            [static_cast<std::size_t>(index)] = index;
        if(index != size){
            result.value[static_cast<std::size_t>(index)]
                [static_cast<std::size_t>(index + 1)] = Value(0);
            result.optimum[static_cast<std::size_t>(index)]
                [static_cast<std::size_t>(index + 1)] = index + 1;
        }
    }
    for(int length = 2; length <= size; ++length){
        for(int left = 0; left + length <= size; ++left){
            const int right = left + length;
            int first = result.optimum[static_cast<std::size_t>(left)]
                [static_cast<std::size_t>(right - 1)];
            int last = result.optimum[static_cast<std::size_t>(left + 1)]
                [static_cast<std::size_t>(right)];
            first = std::max(first, left + 1);
            last = std::min(last, right - 1);
            Value best = infinity;
            int best_split = -1;
            for(int split = first; split <= last; ++split){
                const Value candidate =
                    result.value[static_cast<std::size_t>(left)]
                        [static_cast<std::size_t>(split)]
                    + result.value[static_cast<std::size_t>(split)]
                        [static_cast<std::size_t>(right)];
                if(best_split == -1 || candidate < best){
                    best = candidate;
                    best_split = split;
                }
            }
            result.value[static_cast<std::size_t>(left)]
                [static_cast<std::size_t>(right)] =
                best + std::invoke(interval_cost, left, right);
            result.optimum[static_cast<std::size_t>(left)]
                [static_cast<std::size_t>(right)] = best_split;
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_INTERVAL_PARTITION_KNUTH_OPTIMIZATION_HPP_INCLUDED
