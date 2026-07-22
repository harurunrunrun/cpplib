#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_LOUVAIN_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_LOUVAIN_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <numeric>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "community_detection.hpp"

namespace approximate::graph {
namespace detail {

struct LouvainEdge {
    std::size_t left;
    std::size_t right;
    long double weight_fraction;
};

struct LouvainLevelGraph {
    std::vector<LouvainEdge> edges;
    std::vector<std::vector<std::pair<std::size_t, long double>>> adjacency;
    std::vector<long double> degree_fraction;
    std::vector<long double> loop_fraction;
};

[[nodiscard]] inline LouvainLevelGraph make_louvain_level_graph(const WeightedUndirectedGraph& graph){
    LouvainLevelGraph result;
    result.adjacency.resize(graph.adjacency.size());
    result.degree_fraction.resize(graph.adjacency.size());
    result.loop_fraction.assign(graph.adjacency.size(), 0.0L);
    if(graph.total_edge_weight == 0.0L) return result;
    for(std::size_t vertex = 0; vertex < graph.adjacency.size(); ++vertex){
        result.degree_fraction[vertex] = graph.normalized_degree[vertex] / 2.0L;
    }
    result.edges.reserve(graph.edges.size());
    for(const WeightedUndirectedEdge& edge : graph.edges){
        const long double weight_fraction = edge.weight / graph.total_edge_weight;
        result.edges.push_back({edge.left, edge.right, weight_fraction});
        result.adjacency[edge.left].emplace_back(edge.right, weight_fraction);
        result.adjacency[edge.right].emplace_back(edge.left, weight_fraction);
    }
    return result;
}

[[nodiscard]] inline LouvainLevelGraph contract_louvain_graph(
    const LouvainLevelGraph& graph,
    const std::vector<std::size_t>& labels,
    std::size_t community_count
){
    LouvainLevelGraph result;
    result.adjacency.resize(community_count);
    result.degree_fraction.assign(community_count, 0.0L);
    result.loop_fraction.assign(community_count, 0.0L);
    for(std::size_t vertex = 0; vertex < labels.size(); ++vertex){
        result.degree_fraction[labels[vertex]] += graph.degree_fraction[vertex];
        result.loop_fraction[labels[vertex]] += graph.loop_fraction[vertex];
    }
    std::map<std::pair<std::size_t, std::size_t>, long double> crossing;
    for(const LouvainEdge& edge : graph.edges){
        std::size_t left = labels[edge.left];
        std::size_t right = labels[edge.right];
        if(left == right){
            result.loop_fraction[left] += edge.weight_fraction;
        }else{
            if(left > right) std::swap(left, right);
            crossing[{left, right}] += edge.weight_fraction;
        }
    }
    result.edges.reserve(crossing.size());
    for(const auto& [endpoints, weight] : crossing){
        result.edges.push_back({endpoints.first, endpoints.second, weight});
        result.adjacency[endpoints.first].emplace_back(endpoints.second, weight);
        result.adjacency[endpoints.second].emplace_back(endpoints.first, weight);
    }
    return result;
}

template<class UniformRandomBitGenerator>
[[nodiscard]] std::pair<std::vector<std::size_t>, std::size_t> louvain_local_moving(
    const LouvainLevelGraph& graph,
    UniformRandomBitGenerator& generator,
    std::size_t maximum_passes
){
    const std::size_t vertex_count = graph.adjacency.size();
    std::vector<std::size_t> labels(vertex_count);
    std::vector<std::size_t> order(vertex_count);
    std::iota(labels.begin(), labels.end(), 0);
    std::iota(order.begin(), order.end(), 0);
    std::vector<long double> community_degree = graph.degree_fraction;
    std::size_t passes = 0;
    for(; passes < maximum_passes; ++passes){
        std::shuffle(order.begin(), order.end(), generator);
        bool changed = false;
        for(const std::size_t vertex : order){
            const std::size_t old_label = labels[vertex];
            const long double vertex_degree = graph.degree_fraction[vertex];
            std::map<std::size_t, long double> weight_to_community;
            for(const auto& [neighbor, weight] : graph.adjacency[vertex]){
                weight_to_community[labels[neighbor]] += weight;
            }
            const auto old_iterator = weight_to_community.find(old_label);
            const long double weight_to_old = old_iterator == weight_to_community.end()
                ? 0.0L
                : old_iterator->second;
            std::size_t best_label = old_label;
            long double best_gain = 0.0L;
            for(const auto& [candidate, weight_to_candidate] : weight_to_community){
                if(candidate == old_label) continue;
                const long double gain = weight_to_candidate - weight_to_old
                    - 2.0L * vertex_degree * (community_degree[candidate] - community_degree[old_label])
                    - 2.0L * vertex_degree * vertex_degree;
                if(gain > best_gain || (gain == best_gain && gain > 0.0L && candidate < best_label)){
                    best_gain = gain;
                    best_label = candidate;
                }
            }
            if(best_label != old_label){
                community_degree[old_label] -= vertex_degree;
                community_degree[best_label] += vertex_degree;
                labels[vertex] = best_label;
                changed = true;
            }
        }
        if(!changed){
            ++passes;
            break;
        }
    }
    return {canonical_community_labels(labels), passes};
}

}  // namespace detail

[[nodiscard]] inline CommunityResult louvain_communities(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    std::uint64_t seed,
    std::size_t maximum_levels = 20,
    std::size_t maximum_passes_per_level = 100
){
    const detail::WeightedUndirectedGraph original_graph = detail::make_weighted_undirected_graph(vertex_count, edges);
    std::vector<std::size_t> assignment(vertex_count);
    std::iota(assignment.begin(), assignment.end(), 0);
    if(original_graph.total_edge_weight == 0.0L || maximum_levels == 0 || maximum_passes_per_level == 0){
        return detail::make_community_result(vertex_count, edges, std::move(assignment), 0);
    }
    detail::LouvainLevelGraph graph = detail::make_louvain_level_graph(original_graph);
    std::mt19937_64 generator(seed);
    std::size_t total_passes = 0;
    for(std::size_t level = 0; level < maximum_levels; ++level){
        auto [labels, passes] = detail::louvain_local_moving(graph, generator, maximum_passes_per_level);
        if(passes > std::numeric_limits<std::size_t>::max() - total_passes){
            throw std::overflow_error("the number of Louvain passes is not representable");
        }
        total_passes += passes;
        std::size_t community_count = 0;
        for(const std::size_t label : labels) community_count = std::max(community_count, label + 1);
        for(std::size_t& label : assignment) label = labels[label];
        if(community_count == graph.adjacency.size()) break;
        graph = detail::contract_louvain_graph(graph, labels, community_count);
        if(community_count <= 1) break;
    }
    return detail::make_community_result(vertex_count, edges, std::move(assignment), total_passes);
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_LOUVAIN_HPP_INCLUDED
