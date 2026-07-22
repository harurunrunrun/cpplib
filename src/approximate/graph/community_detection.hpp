#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_COMMUNITY_DETECTION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_COMMUNITY_DETECTION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "weighted_undirected_graph.hpp"

namespace approximate::graph {

struct CommunityResult {
    std::vector<std::size_t> label;
    std::size_t community_count = 0;
    long double modularity = 0.0L;
    std::size_t iterations = 0;
};

[[nodiscard]] inline std::vector<std::size_t> canonical_community_labels(
    const std::vector<std::size_t>& labels
){
    std::map<std::size_t, std::size_t> canonical;
    std::vector<std::size_t> result(labels.size());
    std::size_t next_label = 0;
    for(std::size_t vertex = 0; vertex < labels.size(); ++vertex){
        const auto [iterator, inserted] = canonical.emplace(labels[vertex], next_label);
        if(inserted) ++next_label;
        result[vertex] = iterator->second;
    }
    return result;
}

[[nodiscard]] inline long double community_modularity(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    const std::vector<std::size_t>& labels
){
    if(labels.size() != vertex_count){
        throw std::invalid_argument("community label vector has the wrong size");
    }
    const detail::WeightedUndirectedGraph graph = detail::make_weighted_undirected_graph(vertex_count, edges);
    if(graph.total_edge_weight == 0.0L) return 0.0L;
    const std::vector<std::size_t> canonical = canonical_community_labels(labels);
    std::size_t community_count = 0;
    for(const std::size_t label : canonical) community_count = std::max(community_count, label + 1);
    std::vector<long double> degree_fraction(community_count, 0.0L);
    std::vector<long double> internal_fraction(community_count, 0.0L);
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        degree_fraction[canonical[vertex]] += graph.normalized_degree[vertex] / 2.0L;
    }
    for(const WeightedUndirectedEdge& edge : graph.edges){
        if(canonical[edge.left] == canonical[edge.right]){
            internal_fraction[canonical[edge.left]] += edge.weight / graph.total_edge_weight;
        }
    }
    long double modularity = 0.0L;
    for(std::size_t community = 0; community < community_count; ++community){
        modularity += internal_fraction[community]
            - degree_fraction[community] * degree_fraction[community];
    }
    return modularity;
}

namespace detail {

[[nodiscard]] inline CommunityResult make_community_result(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    std::vector<std::size_t> labels,
    std::size_t iterations
){
    labels = canonical_community_labels(labels);
    std::size_t community_count = 0;
    for(const std::size_t label : labels) community_count = std::max(community_count, label + 1);
    const long double modularity = community_modularity(vertex_count, edges, labels);
    return {std::move(labels), community_count, modularity, iterations};
}

}  // namespace detail
}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_COMMUNITY_DETECTION_HPP_INCLUDED
