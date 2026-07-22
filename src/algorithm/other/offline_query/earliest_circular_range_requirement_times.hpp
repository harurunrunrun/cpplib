#ifndef CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_EARLIEST_CIRCULAR_RANGE_REQUIREMENT_TIMES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_EARLIEST_CIRCULAR_RANGE_REQUIREMENT_TIMES_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "parallel_binary_search.hpp"

struct CircularRangeAddition{
    int left;
    int right;
    long long amount;
};

inline std::vector<int> earliest_circular_range_requirement_times(
    int position_count,
    const std::vector<std::vector<int>>& owned_positions,
    const std::vector<long long>& requirements,
    const std::vector<CircularRangeAddition>& operations
){
    if(position_count < 0 || owned_positions.size() != requirements.size())[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: size mismatch "
            "(earliest_circular_range_requirement_times)."
        );
    }
    for(const auto& positions: owned_positions){
        for(int position: positions){
            if(position < 0 || position_count <= position)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: position out of range "
                    "(earliest_circular_range_requirement_times)."
                );
            }
        }
    }
    for(const auto& operation: operations){
        if(operation.left < 0 || operation.right < 0
            || position_count <= operation.left
            || position_count <= operation.right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: operation out of range "
                "(earliest_circular_range_requirement_times)."
            );
        }
        if(operation.amount < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative operation amount "
                "(earliest_circular_range_requirement_times)."
            );
        }
    }
    std::vector<long long> bit(static_cast<std::size_t>(position_count + 2));
    const auto add = [&](int index, long long value){
        for(int i = index + 1; i <= position_count + 1; i += i & -i){
            bit[static_cast<std::size_t>(i)] += value;
        }
    };
    const auto get = [&](int index){
        long long value = 0;
        for(int i = index + 1; i > 0; i -= i & -i){
            value += bit[static_cast<std::size_t>(i)];
        }
        return value;
    };
    return parallel_binary_search(
        static_cast<int>(requirements.size()),
        static_cast<int>(operations.size()),
        [&]{ std::fill(bit.begin(), bit.end(), 0); },
        [&](int index){
            const auto& operation = operations[static_cast<std::size_t>(index)];
            if(operation.left <= operation.right){
                add(operation.left, operation.amount);
                add(operation.right + 1, -operation.amount);
            }else{
                add(operation.left, operation.amount);
                add(position_count, -operation.amount);
                add(0, operation.amount);
                add(operation.right + 1, -operation.amount);
            }
        },
        [&](int owner){
            long long remaining = requirements[static_cast<std::size_t>(owner)];
            if(remaining <= 0) return true;
            for(int position: owned_positions[static_cast<std::size_t>(owner)]){
                const long long amount = get(position);
                if(amount >= remaining) return true;
                remaining -= amount;
            }
            return false;
        }
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_EARLIEST_CIRCULAR_RANGE_REQUIREMENT_TIMES_HPP_INCLUDED
