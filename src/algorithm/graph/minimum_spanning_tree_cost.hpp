#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_MINIMUM_SPANNING_TREE_COST_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_MINIMUM_SPANNING_TREE_COST_HPP_INCLUDED

#include <limits>
#include <stdexcept>
#include <vector>

#include "kruskal_mst.hpp"

inline long long minimum_spanning_tree_cost(
    int vertex_count,
    const std::vector<KruskalEdge<long long>>& edges
){
    std::vector<KruskalEdge<__int128_t>> widened_edges;
    widened_edges.reserve(edges.size());
    for(const auto& edge: edges){
        widened_edges.push_back({edge.from, edge.to, edge.cost});
    }
    const auto result =
        kruskal_mst<__int128_t>(vertex_count, widened_edges);
    if(!result.connected)[[unlikely]]{
        throw std::invalid_argument(
            "minimum_spanning_tree_cost disconnected graph"
        );
    }
    if(result.cost < std::numeric_limits<long long>::min() ||
       result.cost > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error("minimum_spanning_tree_cost overflow");
    }
    return static_cast<long long>(result.cost);
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_MINIMUM_SPANNING_TREE_COST_HPP_INCLUDED
