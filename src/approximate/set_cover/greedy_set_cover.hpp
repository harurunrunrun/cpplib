#ifndef CPPLIB_SRC_APPROXIMATE_SET_COVER_GREEDY_SET_COVER_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SET_COVER_GREEDY_SET_COVER_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "detail/set_system.hpp"

namespace approximate::set_cover {

struct SetCoverResult {
    std::vector<std::size_t> selected_sets;
    std::vector<std::size_t> uncovered_elements;

    [[nodiscard]] bool covers_universe() const noexcept {
        return uncovered_elements.empty();
    }
};

[[nodiscard]] inline SetCoverResult greedy_set_cover(
    std::size_t universe_size,
    const std::vector<std::vector<std::size_t>>& sets
){
    const detail::SetSystem system = detail::normalize_set_system(universe_size, sets);
    std::vector<bool> covered(universe_size, false);
    std::vector<std::size_t> gains(system.sets.size());
    std::priority_queue<detail::GainEntry, std::vector<detail::GainEntry>, detail::GainEntryLess> heap;
    for(std::size_t set_index = 0; set_index < system.sets.size(); ++set_index){
        gains[set_index] = system.sets[set_index].size();
        heap.push({gains[set_index], set_index});
    }

    SetCoverResult result;
    while(!heap.empty()){
        const detail::GainEntry entry = heap.top();
        heap.pop();
        if(entry.gain != gains[entry.index]) continue;
        if(entry.gain == 0) break;
        result.selected_sets.push_back(entry.index);
        detail::cover_set(
            entry.index,
            system,
            covered,
            gains,
            [&](std::size_t index, std::size_t gain){ heap.push({gain, index}); }
        );
    }
    result.uncovered_elements = detail::collect_elements(covered, false);
    return result;
}

namespace detail {

template<class Weight>
inline void validate_set_weight(const Weight& weight){
    static_assert(std::is_arithmetic_v<Weight> && !std::is_same_v<Weight, bool>);
    if constexpr(std::is_floating_point_v<Weight>){
        if(!std::isfinite(weight) || weight < Weight{0}){
            throw std::invalid_argument("set weights must be finite and nonnegative");
        }
    }else if constexpr(std::is_signed_v<Weight>){
        if(weight < Weight{0}){
            throw std::invalid_argument("set weights must be nonnegative");
        }
    }
}

template<class Weight>
[[nodiscard]] int compare_weight_per_gain(
    const Weight& left_weight,
    std::size_t left_gain,
    const Weight& right_weight,
    std::size_t right_gain
){
    if constexpr(std::is_integral_v<Weight> && sizeof(Weight) <= sizeof(std::uint64_t)){
        using Unsigned = std::make_unsigned_t<Weight>;
        const __uint128_t left = static_cast<__uint128_t>(static_cast<Unsigned>(left_weight)) * right_gain;
        const __uint128_t right = static_cast<__uint128_t>(static_cast<Unsigned>(right_weight)) * left_gain;
        if(left < right) return -1;
        if(left > right) return 1;
        return 0;
    }else{
        const long double left = static_cast<long double>(left_weight) / static_cast<long double>(left_gain);
        const long double right = static_cast<long double>(right_weight) / static_cast<long double>(right_gain);
        if(left < right) return -1;
        if(left > right) return 1;
        return 0;
    }
}

template<class Weight>
struct WeightedGainEntryLess {
    const std::vector<Weight>* weights = nullptr;

    [[nodiscard]] bool operator()(const GainEntry& left, const GainEntry& right) const {
        const int comparison = compare_weight_per_gain(
            (*weights)[left.index], left.gain,
            (*weights)[right.index], right.gain
        );
        if(comparison != 0) return comparison > 0;
        return left.index > right.index;
    }
};

}  // namespace detail

template<class Weight>
[[nodiscard]] SetCoverResult greedy_weighted_set_cover(
    std::size_t universe_size,
    const std::vector<std::vector<std::size_t>>& sets,
    const std::vector<Weight>& weights
){
    static_assert(std::is_arithmetic_v<Weight> && !std::is_same_v<Weight, bool>);
    if(weights.size() != sets.size()){
        throw std::invalid_argument("the number of weights must equal the number of sets");
    }
    for(const Weight& weight : weights) detail::validate_set_weight(weight);
    const detail::SetSystem system = detail::normalize_set_system(universe_size, sets);
    std::vector<bool> covered(universe_size, false);
    std::vector<std::size_t> gains(system.sets.size());
    using Heap = std::priority_queue<
        detail::GainEntry,
        std::vector<detail::GainEntry>,
        detail::WeightedGainEntryLess<Weight>
    >;
    Heap heap(detail::WeightedGainEntryLess<Weight>{&weights});
    for(std::size_t set_index = 0; set_index < system.sets.size(); ++set_index){
        gains[set_index] = system.sets[set_index].size();
        if(gains[set_index] != 0) heap.push({gains[set_index], set_index});
    }

    SetCoverResult result;
    while(!heap.empty()){
        const detail::GainEntry entry = heap.top();
        heap.pop();
        if(entry.gain != gains[entry.index]) continue;
        if(entry.gain == 0) break;
        result.selected_sets.push_back(entry.index);
        detail::cover_set(
            entry.index,
            system,
            covered,
            gains,
            [&](std::size_t index, std::size_t gain){
                if(gain != 0) heap.push({gain, index});
            }
        );
    }
    result.uncovered_elements = detail::collect_elements(covered, false);
    return result;
}

}  // namespace approximate::set_cover

#endif  // CPPLIB_SRC_APPROXIMATE_SET_COVER_GREEDY_SET_COVER_HPP_INCLUDED
