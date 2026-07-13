#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace approximate::set_cover::detail {

struct SetSystem {
    std::vector<std::vector<std::size_t>> sets;
    std::vector<std::vector<std::size_t>> incidence;
};

[[nodiscard]] inline SetSystem normalize_set_system(
    std::size_t universe_size,
    const std::vector<std::vector<std::size_t>>& input
){
    SetSystem result;
    result.sets = input;
    result.incidence.resize(universe_size);
    for(std::size_t set_index = 0; set_index < result.sets.size(); ++set_index){
        auto& set = result.sets[set_index];
        for(const std::size_t element : set){
            if(element >= universe_size){
                throw std::out_of_range("set element is outside the universe");
            }
        }
        std::sort(set.begin(), set.end());
        set.erase(std::unique(set.begin(), set.end()), set.end());
        for(const std::size_t element : set){
            result.incidence[element].push_back(set_index);
        }
    }
    return result;
}

struct GainEntry {
    std::size_t gain;
    std::size_t index;
};

struct GainEntryLess {
    [[nodiscard]] bool operator()(const GainEntry& left, const GainEntry& right) const noexcept {
        if(left.gain != right.gain) return left.gain < right.gain;
        return left.index > right.index;
    }
};

inline void cover_set(
    std::size_t selected,
    const SetSystem& system,
    std::vector<bool>& covered,
    std::vector<std::size_t>& gains,
    auto&& on_gain_changed
){
    for(const std::size_t element : system.sets[selected]){
        if(covered[element]) continue;
        covered[element] = true;
        for(const std::size_t set_index : system.incidence[element]){
            --gains[set_index];
            on_gain_changed(set_index, gains[set_index]);
        }
    }
}

[[nodiscard]] inline std::vector<std::size_t> collect_elements(
    const std::vector<bool>& covered,
    bool desired_state
){
    std::vector<std::size_t> result;
    for(std::size_t element = 0; element < covered.size(); ++element){
        if(covered[element] == desired_state) result.push_back(element);
    }
    return result;
}

}  // namespace approximate::set_cover::detail
