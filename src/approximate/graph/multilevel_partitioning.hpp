#pragma once

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "fiduccia_mattheyses.hpp"
#include "spectral_bisection.hpp"

namespace approximate::graph {

struct MultilevelPartitionResult {
    std::vector<unsigned char> side;
    long double cut_weight = 0.0L;
    std::size_t coarsening_levels = 0;
};

namespace detail {

struct GraphCoarseningLevel {
    std::size_t vertex_count;
    std::vector<WeightedUndirectedEdge> edges;
    std::vector<std::size_t> fine_to_coarse;
};

[[nodiscard]] inline std::vector<std::size_t> heavy_edge_matching(const WeightedUndirectedGraph& graph){
    const std::size_t vertex_count = graph.adjacency.size();
    std::vector<bool> matched(vertex_count, false);
    std::vector<std::size_t> fine_to_coarse(vertex_count, vertex_count);
    std::size_t coarse_count = 0;
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        if(matched[vertex]) continue;
        std::size_t partner = vertex_count;
        long double best_weight = -1.0L;
        for(const WeightedNeighbor& neighbor : graph.adjacency[vertex]){
            if(matched[neighbor.vertex]) continue;
            if(neighbor.normalized_weight > best_weight
               || (neighbor.normalized_weight == best_weight && neighbor.vertex < partner)){
                best_weight = neighbor.normalized_weight;
                partner = neighbor.vertex;
            }
        }
        matched[vertex] = true;
        fine_to_coarse[vertex] = coarse_count;
        if(partner != vertex_count){
            matched[partner] = true;
            fine_to_coarse[partner] = coarse_count;
        }
        ++coarse_count;
    }
    return fine_to_coarse;
}

[[nodiscard]] inline std::vector<WeightedUndirectedEdge> contract_edges(
    const WeightedUndirectedGraph& graph,
    const std::vector<std::size_t>& fine_to_coarse
){
    std::vector<WeightedUndirectedEdge> edges;
    edges.reserve(graph.edges.size());
    for(const WeightedUndirectedEdge& edge : graph.edges){
        const std::size_t left = fine_to_coarse[edge.left];
        const std::size_t right = fine_to_coarse[edge.right];
        if(left != right) edges.push_back({left, right, edge.weight});
    }
    return edges;
}

}  // namespace detail

[[nodiscard]] inline MultilevelPartitionResult multilevel_graph_partitioning(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    std::size_t coarsest_vertex_count = 40,
    std::size_t maximum_imbalance = 2,
    std::size_t maximum_levels = 32,
    std::size_t refinement_passes = 4
){
    if(coarsest_vertex_count < 2){
        throw std::invalid_argument("coarsest vertex count must be at least two");
    }
    detail::validate_maximum_imbalance(vertex_count, maximum_imbalance);
    detail::WeightedUndirectedGraph current_graph = detail::make_weighted_undirected_graph(vertex_count, edges);
    std::size_t current_vertex_count = vertex_count;
    std::vector<detail::GraphCoarseningLevel> levels;
    levels.reserve(std::min(maximum_levels, current_vertex_count));

    while(current_vertex_count > coarsest_vertex_count && levels.size() < maximum_levels){
        std::vector<std::size_t> mapping = detail::heavy_edge_matching(current_graph);
        std::size_t coarse_vertex_count = 0;
        for(const std::size_t value : mapping) coarse_vertex_count = std::max(coarse_vertex_count, value + 1);
        if(coarse_vertex_count >= current_vertex_count) break;
        std::vector<WeightedUndirectedEdge> coarse_edges = detail::contract_edges(current_graph, mapping);
        levels.push_back({current_vertex_count, current_graph.edges, std::move(mapping)});
        current_vertex_count = coarse_vertex_count;
        current_graph = detail::make_weighted_undirected_graph(current_vertex_count, coarse_edges);
    }

    std::vector<unsigned char> side = spectral_bisection(current_vertex_count, current_graph.edges).side;
    for(std::size_t level_index = levels.size(); level_index > 0; --level_index){
        const detail::GraphCoarseningLevel& level = levels[level_index - 1];
        std::vector<unsigned char> projected(level.vertex_count);
        for(std::size_t vertex = 0; vertex < level.vertex_count; ++vertex){
            projected[vertex] = side[level.fine_to_coarse[vertex]];
        }
        const detail::WeightedUndirectedGraph fine_graph = detail::make_weighted_undirected_graph(
            level.vertex_count, level.edges
        );
        const std::size_t level_maximum_imbalance = std::max(maximum_imbalance, level.vertex_count % 2);
        detail::rebalance_partition(fine_graph, projected, level_maximum_imbalance);
        side = fiduccia_mattheyses(
            level.vertex_count, level.edges, std::move(projected), level_maximum_imbalance, refinement_passes
        ).side;
    }
    const detail::WeightedUndirectedGraph original_graph = detail::make_weighted_undirected_graph(vertex_count, edges);
    detail::rebalance_partition(original_graph, side, maximum_imbalance);
    side = fiduccia_mattheyses(
        vertex_count, original_graph.edges, std::move(side), maximum_imbalance, refinement_passes
    ).side;
    BipartitionResult result = detail::make_bipartition_result(original_graph, std::move(side));
    return {std::move(result.side), result.cut_weight, levels.size()};
}

}  // namespace approximate::graph
