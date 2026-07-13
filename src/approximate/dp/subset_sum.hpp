#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "knapsack.hpp"

namespace approximate::dp {

struct SubsetSumResult {
    std::vector<std::size_t> selected_indices;
    std::uint64_t sum = 0;
};

namespace detail {

struct SubsetTrace {
    std::size_t parent = 0;
    std::size_t item = 0;
};

struct SubsetState {
    std::uint64_t sum = 0;
    std::size_t trace = 0;
};

template<class Integer, class Capacity>
[[nodiscard]] std::pair<std::vector<std::uint64_t>, std::uint64_t> normalize_subset_sum(
    const std::vector<Integer>& values,
    Capacity capacity
){
    static_assert(std::is_integral_v<Integer> && !std::is_same_v<Integer, bool>);
    static_assert(std::is_integral_v<Capacity> && !std::is_same_v<Capacity, bool>);
    const std::uint64_t normalized_capacity = nonnegative_integer(
        capacity,
        "capacity must be nonnegative"
    );
    std::vector<std::uint64_t> normalized;
    normalized.reserve(values.size());
    for(const Integer value : values){
        normalized.push_back(nonnegative_integer(value, "subset values must be nonnegative"));
    }
    return {std::move(normalized), normalized_capacity};
}

[[nodiscard]] inline SubsetSumResult recover_subset_sum(
    const SubsetState& state,
    const std::vector<SubsetTrace>& traces
){
    SubsetSumResult result;
    result.sum = state.sum;
    std::size_t trace = state.trace;
    while(trace != 0){
        result.selected_indices.push_back(traces[trace].item);
        trace = traces[trace].parent;
    }
    std::sort(result.selected_indices.begin(), result.selected_indices.end());
    return result;
}

template<class Keep>
[[nodiscard]] SubsetSumResult subset_sum_merge_dp(
    const std::vector<std::uint64_t>& values,
    std::uint64_t capacity,
    Keep keep
){
    std::vector<SubsetTrace> traces(1);
    std::vector<SubsetState> states{{0, 0}};
    for(std::size_t item = 0; item < values.size(); ++item){
        const std::uint64_t value = values[item];
        std::vector<SubsetState> next;
        next.reserve(states.size() <= std::numeric_limits<std::size_t>::max() / 2
                         ? states.size() * 2
                         : states.size());
        std::size_t plain = 0;
        std::size_t shifted = 0;
        bool have_last = false;
        std::uint64_t last = 0;
        while(plain < states.size() || shifted < states.size()){
            const bool shifted_valid = shifted < states.size() &&
                                       value <= capacity &&
                                       states[shifted].sum <= capacity - value;
            const bool plain_valid = plain < states.size();
            if(!plain_valid && !shifted_valid) break;
            const std::uint64_t plain_sum = plain_valid ? states[plain].sum : 0;
            const std::uint64_t shifted_sum = shifted_valid ? states[shifted].sum + value : 0;
            const bool use_shifted = shifted_valid &&
                                     (!plain_valid || shifted_sum < plain_sum);
            const std::uint64_t candidate_sum = use_shifted ? shifted_sum : plain_sum;
            const std::size_t parent_trace = use_shifted
                ? states[shifted].trace
                : states[plain].trace;
            if(plain_valid && shifted_valid && shifted_sum == plain_sum){
                ++plain;
                ++shifted;
            }else if(use_shifted){
                ++shifted;
            }else{
                ++plain;
            }
            if(have_last && !keep(last, candidate_sum)) continue;
            std::size_t trace = parent_trace;
            if(use_shifted){
                traces.push_back({parent_trace, item});
                trace = traces.size() - 1;
            }
            next.push_back({candidate_sum, trace});
            last = candidate_sum;
            have_last = true;
        }
        states = std::move(next);
    }
    return recover_subset_sum(states.back(), traces);
}

}  // namespace detail

template<class Integer, class Capacity>
[[nodiscard]] SubsetSumResult sparse_subset_sum(
    const std::vector<Integer>& values,
    Capacity capacity
){
    auto [normalized, normalized_capacity] = detail::normalize_subset_sum(values, capacity);
    return detail::subset_sum_merge_dp(
        normalized,
        normalized_capacity,
        [](std::uint64_t last, std::uint64_t candidate){ return candidate != last; }
    );
}

template<class Integer, class Capacity>
[[nodiscard]] SubsetSumResult subset_sum_trimming_fptas(
    const std::vector<Integer>& values,
    Capacity capacity,
    long double epsilon
){
    if(!std::isfinite(epsilon) || !(epsilon > 0.0L && epsilon < 1.0L)){
        throw std::invalid_argument("epsilon must be finite and in (0, 1)");
    }
    auto [normalized, normalized_capacity] = detail::normalize_subset_sum(values, capacity);
    if(normalized.empty()) return {};
    const long double delta = epsilon /
        (2.0L * static_cast<long double>(normalized.size()));
    return detail::subset_sum_merge_dp(
        normalized,
        normalized_capacity,
        [delta](std::uint64_t last, std::uint64_t candidate){
            return static_cast<long double>(candidate) >
                   static_cast<long double>(last) * (1.0L + delta);
        }
    );
}

}  // namespace approximate::dp
