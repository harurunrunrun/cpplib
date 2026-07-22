#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_CHINESE_WHISPERS_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_CHINESE_WHISPERS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <numeric>
#include <random>
#include <vector>

#include "community_detection.hpp"

namespace approximate::graph {

[[nodiscard]] inline CommunityResult chinese_whispers(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    std::uint64_t seed,
    std::size_t maximum_iterations = 100
){
    const detail::WeightedUndirectedGraph graph = detail::make_weighted_undirected_graph(vertex_count, edges);
    std::vector<std::size_t> labels(vertex_count);
    std::vector<std::size_t> order(vertex_count);
    std::iota(labels.begin(), labels.end(), 0);
    std::iota(order.begin(), order.end(), 0);
    std::mt19937_64 generator(seed);
    std::size_t iterations = 0;
    for(; iterations < maximum_iterations; ++iterations){
        std::shuffle(order.begin(), order.end(), generator);
        bool changed = false;
        for(const std::size_t vertex : order){
            if(graph.adjacency[vertex].empty()) continue;
            std::map<std::size_t, long double> score;
            for(const detail::WeightedNeighbor& neighbor : graph.adjacency[vertex]){
                score[labels[neighbor.vertex]] += neighbor.normalized_weight;
            }
            long double best_score = -1.0L;
            std::vector<std::size_t> candidates;
            for(const auto& entry : score){
                if(entry.second > best_score){
                    best_score = entry.second;
                    candidates.assign(1, entry.first);
                }else if(entry.second == best_score){
                    candidates.push_back(entry.first);
                }
            }
            std::uniform_int_distribution<std::size_t> choose(0, candidates.size() - 1);
            const std::size_t best_label = candidates[choose(generator)];
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

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_CHINESE_WHISPERS_HPP_INCLUDED
