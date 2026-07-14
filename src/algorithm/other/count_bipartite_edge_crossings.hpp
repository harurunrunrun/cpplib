#pragma once

#include <algorithm>
#include <utility>
#include <vector>

#include "inversion_count.hpp"

inline long long count_bipartite_edge_crossings(
    std::vector<std::pair<int, int>> edges
){
    std::sort(edges.begin(), edges.end());
    std::vector<int> right_endpoint;
    right_endpoint.reserve(edges.size());
    for(const auto [left, right]: edges){
        (void)left;
        right_endpoint.push_back(right);
    }
    return inversion_count(right_endpoint);
}
