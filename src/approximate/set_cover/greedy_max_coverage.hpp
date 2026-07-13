#pragma once

#include <cstddef>
#include <queue>
#include <vector>

#include "detail/set_system.hpp"

namespace approximate::set_cover {

struct MaxCoverageResult {
    std::vector<std::size_t> selected_sets;
    std::vector<std::size_t> covered_elements;
    std::vector<std::size_t> uncovered_elements;
};

[[nodiscard]] inline MaxCoverageResult greedy_max_coverage(
    std::size_t universe_size,
    const std::vector<std::vector<std::size_t>>& sets,
    std::size_t maximum_sets
){
    const detail::SetSystem system = detail::normalize_set_system(universe_size, sets);
    std::vector<bool> covered(universe_size, false);
    std::vector<std::size_t> gains(system.sets.size());
    std::priority_queue<detail::GainEntry, std::vector<detail::GainEntry>, detail::GainEntryLess> heap;
    for(std::size_t set_index = 0; set_index < system.sets.size(); ++set_index){
        gains[set_index] = system.sets[set_index].size();
        heap.push({gains[set_index], set_index});
    }

    MaxCoverageResult result;
    while(result.selected_sets.size() < maximum_sets && !heap.empty()){
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
    result.covered_elements = detail::collect_elements(covered, true);
    result.uncovered_elements = detail::collect_elements(covered, false);
    return result;
}

}  // namespace approximate::set_cover
