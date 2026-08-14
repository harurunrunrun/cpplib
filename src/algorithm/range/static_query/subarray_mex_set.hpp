#ifndef CPPLIB_SRC_ALGORITHM_RANGE_STATIC_QUERY_SUBARRAY_MEX_SET_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_STATIC_QUERY_SUBARRAY_MEX_SET_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

inline std::vector<std::size_t> subarray_mex_values(
    const std::vector<std::int64_t>& values,
    std::size_t first = 0
){
    if(first > 1){
        throw std::invalid_argument("subarray_mex_values: first must be zero or one");
    }
    const std::size_t candidate_count = values.size() + 1;
    std::size_t leaf_count = 1;
    while(leaf_count < candidate_count) leaf_count <<= 1;
    std::vector<std::size_t> minimum_last_plus_one(leaf_count << 1, 0);
    std::vector<std::size_t> previous_plus_one(candidate_count, 0);
    std::vector<bool> possible(candidate_count, false);

    const auto set_last = [&](std::size_t position, std::size_t last_plus_one){
        std::size_t node = leaf_count + position;
        minimum_last_plus_one[node] = last_plus_one;
        while(node > 1){
            node >>= 1;
            minimum_last_plus_one[node] = std::min(minimum_last_plus_one[node << 1], minimum_last_plus_one[node << 1 | 1]);
        }
    };
    const auto prefix_minimum = [&](std::size_t right){
        std::size_t answer = values.size() + 1;
        std::size_t left_node = leaf_count;
        std::size_t right_node = leaf_count + right;
        while(left_node < right_node){
            if(left_node & 1U) answer = std::min(answer, minimum_last_plus_one[left_node++]);
            if(right_node & 1U) answer = std::min(answer, minimum_last_plus_one[--right_node]);
            left_node >>= 1;
            right_node >>= 1;
        }
        return answer;
    };
    const auto index_of = [first, candidate_count](std::int64_t value) -> std::size_t{
        if(value < static_cast<std::int64_t>(first)) return candidate_count;
        const std::uint64_t offset = static_cast<std::uint64_t>(value) - first;
        return offset < candidate_count ? static_cast<std::size_t>(offset) : candidate_count;
    };

    for(std::size_t position = 0; position < values.size(); position++){
        const std::size_t index = index_of(values[position]);
        if(index == candidate_count) continue;
        const bool nonempty_gap = position + 1 - previous_plus_one[index] > 1;
        const bool contains_all_smaller = index == 0 || prefix_minimum(index) > previous_plus_one[index];
        if(nonempty_gap && contains_all_smaller) possible[index] = true;
        previous_plus_one[index] = position + 1;
        set_last(index, position + 1);
    }
    for(std::size_t index = 0; index < candidate_count; index++){
        const bool nonempty_gap = values.size() + 1 - previous_plus_one[index] > 1;
        const bool contains_all_smaller = index == 0 || prefix_minimum(index) > previous_plus_one[index];
        if(nonempty_gap && contains_all_smaller) possible[index] = true;
    }

    std::vector<std::size_t> answer;
    for(std::size_t index = 0; index < candidate_count; index++){
        if(possible[index]) answer.push_back(first + index);
    }
    return answer;
}

inline std::size_t mex_of_subarray_mex_values(
    const std::vector<std::int64_t>& values,
    std::size_t first = 0
){
    const std::vector<std::size_t> possible = subarray_mex_values(values, first);
    std::size_t answer = first;
    for(const std::size_t value: possible){
        if(value == answer) ++answer;
        else if(value > answer) break;
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_STATIC_QUERY_SUBARRAY_MEX_SET_HPP_INCLUDED
