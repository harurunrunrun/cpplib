#pragma once

#include <stdexcept>
#include <tuple>
#include <vector>

#include "kruskal_mst.hpp"

inline long long minimum_spanning_tree_total_weight(
    int vertex_count,
    const std::vector<std::tuple<int, int, long long>>& edges
){
    if(vertex_count <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(minimum_spanning_tree_total_weight)."
        );
    }
    std::vector<KruskalEdge<long long>> converted;
    converted.reserve(edges.size());
    for(const auto& [left, right, weight]: edges){
        converted.push_back({left, right, weight});
    }
    const auto result = kruskal_mst(vertex_count, converted);
    if(!result.connected)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: disconnected graph "
            "(minimum_spanning_tree_total_weight)."
        );
    }
    return result.cost;
}
