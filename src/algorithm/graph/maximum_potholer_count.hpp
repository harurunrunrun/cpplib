#pragma once

#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../matching/dinic.hpp"

inline int maximum_potholer_count(
    int chamber_count,
    const std::vector<std::pair<int, int>>& directed_corridors
){
    if(chamber_count < 2)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (maximum_potholer_count)."
        );
    }
    Dinic<int> flow(chamber_count);
    const int sink = chamber_count - 1;
    const int infinity = std::numeric_limits<int>::max() / 4;
    for(const auto& [from, to]: directed_corridors){
        if(from < 0 || chamber_count <= from || to < 0 || chamber_count <= to)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(maximum_potholer_count)."
            );
        }
        int capacity = (from == 0 || to == sink) ? 1 : infinity;
        flow.add_edge(from, to, capacity);
    }
    return flow.max_flow(0, sink);
}
