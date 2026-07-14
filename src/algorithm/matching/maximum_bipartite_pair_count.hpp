#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

#include "bipartite_matching.hpp"

inline int maximum_bipartite_pair_count(
    int left_size,
    int right_size,
    const std::vector<std::pair<int, int>>& allowed_pairs
){
    if(left_size < 0 || right_size < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(maximum_bipartite_pair_count)."
        );
    }
    BipartiteMatching matching(left_size, right_size);
    for(const auto& [left, right]: allowed_pairs){
        matching.add_edge(left, right);
    }
    return matching.solve().size;
}
