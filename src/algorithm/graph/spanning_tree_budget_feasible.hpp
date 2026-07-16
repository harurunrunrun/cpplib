#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "kruskal_mst.hpp"

inline bool spanning_tree_budget_feasible(
    int vertex_count,
    const std::vector<KruskalEdge<long long>>& edges,
    long long budget
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::invalid_argument(
            "spanning_tree_budget_feasible negative vertex count"
        );
    }

    std::vector<KruskalEdge<__int128_t>> widened_edges;
    widened_edges.reserve(edges.size());
    for(const auto& edge: edges){
        if(edge.from < 0 || vertex_count <= edge.from ||
           edge.to < 0 || vertex_count <= edge.to)[[unlikely]]{
            throw std::out_of_range(
                "spanning_tree_budget_feasible edge is out of range"
            );
        }
        widened_edges.push_back({edge.from, edge.to, edge.cost});
    }

    const auto result =
        kruskal_mst<__int128_t>(vertex_count, widened_edges);
    return result.connected &&
        result.cost <= static_cast<__int128_t>(budget);
}
