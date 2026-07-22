#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_GREEDY_MODULARITY_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_GREEDY_MODULARITY_HPP_INCLUDED

#include <cstddef>
#include <map>
#include <numeric>
#include <utility>
#include <vector>

#include "community_detection.hpp"

namespace approximate::graph {

[[nodiscard]] inline CommunityResult greedy_modularity(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    std::size_t maximum_merges = static_cast<std::size_t>(-1)
){
    const detail::WeightedUndirectedGraph graph = detail::make_weighted_undirected_graph(vertex_count, edges);
    std::vector<std::size_t> labels(vertex_count);
    std::iota(labels.begin(), labels.end(), 0);
    if(graph.total_edge_weight == 0.0L){
        return detail::make_community_result(vertex_count, edges, std::move(labels), 0);
    }
    std::vector<long double> degree_fraction(vertex_count, 0.0L);
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        degree_fraction[vertex] = graph.normalized_degree[vertex] / 2.0L;
    }

    std::size_t merges = 0;
    while(merges < maximum_merges){
        std::map<std::pair<std::size_t, std::size_t>, long double> between_fraction;
        for(const WeightedUndirectedEdge& edge : graph.edges){
            std::size_t left = labels[edge.left];
            std::size_t right = labels[edge.right];
            if(left == right) continue;
            if(left > right) std::swap(left, right);
            between_fraction[{left, right}] += edge.weight / graph.total_edge_weight;
        }
        bool found = false;
        std::pair<std::size_t, std::size_t> best_pair{};
        long double best_gain = 0.0L;
        for(const auto& [communities, between] : between_fraction){
            const long double gain = between
                - 2.0L * degree_fraction[communities.first] * degree_fraction[communities.second];
            if(gain > best_gain || (gain == best_gain && gain > 0.0L && communities < best_pair)){
                found = true;
                best_gain = gain;
                best_pair = communities;
            }
        }
        if(!found || best_gain <= 0.0L) break;
        for(std::size_t& label : labels){
            if(label == best_pair.second) label = best_pair.first;
        }
        degree_fraction[best_pair.first] += degree_fraction[best_pair.second];
        degree_fraction[best_pair.second] = 0.0L;
        ++merges;
    }
    return detail::make_community_result(vertex_count, edges, std::move(labels), merges);
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_GREEDY_MODULARITY_HPP_INCLUDED
