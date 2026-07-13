#pragma once

#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include "graph_partition.hpp"

namespace approximate::graph {

[[nodiscard]] inline BipartitionResult kernighan_lin(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    std::vector<unsigned char> initial_side,
    std::size_t maximum_passes = 20
){
    const detail::WeightedUndirectedGraph graph = detail::make_weighted_undirected_graph(vertex_count, edges);
    detail::validate_partition(initial_side, vertex_count);
    std::vector<unsigned char> side = std::move(initial_side);
    if(vertex_count < 2 || graph.total_edge_weight == 0.0L || maximum_passes == 0){
        return detail::make_bipartition_result(graph, std::move(side));
    }

    for(std::size_t pass = 0; pass < maximum_passes; ++pass){
        std::vector<long double> difference(vertex_count, 0.0L);
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            for(const detail::WeightedNeighbor& neighbor : graph.adjacency[vertex]){
                difference[vertex] += side[vertex] == side[neighbor.vertex]
                    ? -neighbor.normalized_weight
                    : neighbor.normalized_weight;
            }
        }
        std::vector<bool> locked(vertex_count, false);
        std::vector<std::pair<std::size_t, std::size_t>> swaps;
        std::vector<long double> gains;
        const std::size_t pair_count = [&side]{
            std::size_t zeros = 0;
            for(const unsigned char value : side) zeros += value == 0 ? 1U : 0U;
            return std::min(zeros, side.size() - zeros);
        }();
        swaps.reserve(pair_count);
        gains.reserve(pair_count);

        for(std::size_t step = 0; step < pair_count; ++step){
            std::size_t best_left = vertex_count;
            std::size_t best_right = vertex_count;
            long double best_gain = -std::numeric_limits<long double>::infinity();
            for(std::size_t left = 0; left < vertex_count; ++left){
                if(locked[left] || side[left] != 0) continue;
                for(std::size_t right = 0; right < vertex_count; ++right){
                    if(locked[right] || side[right] != 1) continue;
                    const long double gain = difference[left] + difference[right]
                        - 2.0L * detail::normalized_pair_weight(graph, left, right);
                    if(gain > best_gain ||
                       (gain == best_gain && std::pair{left, right} < std::pair{best_left, best_right})){
                        best_gain = gain;
                        best_left = left;
                        best_right = right;
                    }
                }
            }
            if(best_left == vertex_count) break;
            locked[best_left] = true;
            locked[best_right] = true;
            swaps.emplace_back(best_left, best_right);
            gains.push_back(best_gain);

            for(const detail::WeightedNeighbor& neighbor : graph.adjacency[best_left]){
                if(locked[neighbor.vertex]) continue;
                difference[neighbor.vertex] += side[neighbor.vertex] == 0
                    ? 2.0L * neighbor.normalized_weight
                    : -2.0L * neighbor.normalized_weight;
            }
            for(const detail::WeightedNeighbor& neighbor : graph.adjacency[best_right]){
                if(locked[neighbor.vertex]) continue;
                difference[neighbor.vertex] += side[neighbor.vertex] == 1
                    ? 2.0L * neighbor.normalized_weight
                    : -2.0L * neighbor.normalized_weight;
            }
        }

        long double cumulative_gain = 0.0L;
        long double best_cumulative_gain = 0.0L;
        std::size_t best_prefix = 0;
        for(std::size_t index = 0; index < gains.size(); ++index){
            cumulative_gain += gains[index];
            if(cumulative_gain > best_cumulative_gain){
                best_cumulative_gain = cumulative_gain;
                best_prefix = index + 1;
            }
        }
        if(best_prefix == 0) break;

        std::vector<unsigned char> candidate = side;
        for(std::size_t index = 0; index < best_prefix; ++index){
            candidate[swaps[index].first] = 1;
            candidate[swaps[index].second] = 0;
        }
        if(detail::normalized_cut_weight(graph, candidate) >= detail::normalized_cut_weight(graph, side)) break;
        side = std::move(candidate);
    }
    return detail::make_bipartition_result(graph, std::move(side));
}

[[nodiscard]] inline BipartitionResult kernighan_lin(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    std::size_t maximum_passes = 20
){
    return kernighan_lin(vertex_count, edges, balanced_bipartition(vertex_count), maximum_passes);
}

}  // namespace approximate::graph
