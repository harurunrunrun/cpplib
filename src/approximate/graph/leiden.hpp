#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_LEIDEN_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_LEIDEN_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <queue>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "louvain.hpp"

namespace approximate::graph {

struct LeidenResult {
    std::vector<std::size_t> labels;
    std::size_t community_count = 0;
    long double modularity = 0.0L;
    std::size_t iterations = 0;
    std::size_t levels = 0;
};

namespace detail {

[[nodiscard]] inline std::vector<std::size_t> refine_connected_communities(
    const LouvainLevelGraph& graph,
    const std::vector<std::size_t>& labels
){
    const std::size_t vertex_count = graph.adjacency.size();
    std::vector<std::size_t> refined(vertex_count, vertex_count);
    std::size_t next_label = 0;
    std::queue<std::size_t> queue;
    for(std::size_t start = 0; start < vertex_count; ++start){
        if(refined[start] != vertex_count) continue;
        refined[start] = next_label;
        queue.push(start);
        while(!queue.empty()){
            const std::size_t vertex = queue.front();
            queue.pop();
            for(const auto& [neighbor, weight] : graph.adjacency[vertex]){
                if(weight > 0.0L && labels[neighbor] == labels[start]
                   && refined[neighbor] == vertex_count){
                    refined[neighbor] = next_label;
                    queue.push(neighbor);
                }
            }
        }
        ++next_label;
    }
    return refined;
}

}  // namespace detail

[[nodiscard]] inline LeidenResult leiden_communities(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    std::uint64_t seed,
    std::size_t maximum_levels = 20,
    std::size_t maximum_passes_per_level = 100
){
    const detail::WeightedUndirectedGraph original_graph = detail::make_weighted_undirected_graph(
        vertex_count, edges
    );
    std::vector<std::size_t> assignment(vertex_count);
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex) assignment[vertex] = vertex;
    LeidenResult result;
    if(original_graph.total_edge_weight == 0.0L || maximum_levels == 0
       || maximum_passes_per_level == 0){
        const CommunityResult communities = detail::make_community_result(
            vertex_count, edges, std::move(assignment), 0
        );
        result.labels = communities.label;
        result.community_count = communities.community_count;
        result.modularity = communities.modularity;
        return result;
    }

    detail::LouvainLevelGraph graph = detail::make_louvain_level_graph(original_graph);
    std::mt19937_64 generator(seed);
    for(std::size_t level = 0; level < maximum_levels; ++level){
        auto [local_labels, passes] = detail::louvain_local_moving(
            graph, generator, maximum_passes_per_level
        );
        if(passes > std::numeric_limits<std::size_t>::max() - result.iterations){
            throw std::overflow_error("the number of Leiden passes is not representable");
        }
        result.iterations += passes;
        std::vector<std::size_t> refined = detail::refine_connected_communities(
            graph, local_labels
        );
        std::size_t refined_count = 0;
        for(const std::size_t label : refined) refined_count = std::max(refined_count, label + 1);
        for(std::size_t& label : assignment) label = refined[label];
        ++result.levels;
        if(refined_count == graph.adjacency.size()) break;
        graph = detail::contract_louvain_graph(graph, refined, refined_count);
        if(refined_count <= 1) break;
    }

    const CommunityResult communities = detail::make_community_result(
        vertex_count, edges, std::move(assignment), result.iterations
    );
    result.labels = communities.label;
    result.community_count = communities.community_count;
    result.modularity = communities.modularity;
    return result;
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_LEIDEN_HPP_INCLUDED
