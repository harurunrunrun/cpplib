#pragma once

#include <cstddef>
#include <map>
#include <numeric>
#include <vector>

#include "community_detection.hpp"

namespace approximate::graph {

[[nodiscard]] inline CommunityResult label_propagation(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    std::size_t maximum_iterations = 100
){
    const detail::WeightedUndirectedGraph graph = detail::make_weighted_undirected_graph(vertex_count, edges);
    std::vector<std::size_t> labels(vertex_count);
    std::iota(labels.begin(), labels.end(), 0);
    std::size_t iterations = 0;
    for(; iterations < maximum_iterations; ++iterations){
        bool changed = false;
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            if(graph.adjacency[vertex].empty()) continue;
            std::map<std::size_t, long double> score;
            for(const detail::WeightedNeighbor& neighbor : graph.adjacency[vertex]){
                score[labels[neighbor.vertex]] += neighbor.normalized_weight;
            }
            long double best_score = -1.0L;
            for(const auto& entry : score) best_score = std::max(best_score, entry.second);
            std::size_t best_label = labels[vertex];
            const auto current = score.find(labels[vertex]);
            if(current == score.end() || current->second != best_score){
                for(const auto& [label, value] : score){
                    if(value == best_score){
                        best_label = label;
                        break;
                    }
                }
            }
            if(best_label != labels[vertex]){
                labels[vertex] = best_label;
                changed = true;
            }
        }
        if(!changed){
            ++iterations;
            break;
        }
    }
    return detail::make_community_result(vertex_count, edges, std::move(labels), iterations);
}

}  // namespace approximate::graph
