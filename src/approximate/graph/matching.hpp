#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_MATCHING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_MATCHING_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "detail/undirected_graph.hpp"

namespace approximate::graph {

struct MatchingResult {
    std::vector<std::size_t> edge_indices;
    std::vector<std::size_t> mate;
};

[[nodiscard]] inline MatchingResult greedy_matching(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    MatchingResult result;
    result.mate.assign(vertex_count, vertex_count);
    for(std::size_t edge_index = 0; edge_index < edges.size(); ++edge_index){
        const auto [left, right] = edges[edge_index];
        detail::validate_endpoint(vertex_count, left);
        detail::validate_endpoint(vertex_count, right);
        if(left == right) continue;
        if(result.mate[left] == vertex_count && result.mate[right] == vertex_count){
            result.mate[left] = right;
            result.mate[right] = left;
            result.edge_indices.push_back(edge_index);
        }
    }
    return result;
}

template<class Weight>
[[nodiscard]] MatchingResult greedy_weighted_matching(
    std::size_t vertex_count,
    const std::vector<std::tuple<std::size_t, std::size_t, Weight>>& edges
){
    static_assert(std::is_arithmetic_v<Weight> && !std::is_same_v<Weight, bool>);
    std::vector<std::size_t> order;
    order.reserve(edges.size());
    for(std::size_t edge_index = 0; edge_index < edges.size(); ++edge_index){
        const auto& [left, right, weight] = edges[edge_index];
        detail::validate_endpoint(vertex_count, left);
        detail::validate_endpoint(vertex_count, right);
        if constexpr(std::is_floating_point_v<Weight>){
            if(!std::isfinite(weight)){
                throw std::invalid_argument("matching weights must be finite");
            }
        }
        if(left != right && weight > Weight{0}) order.push_back(edge_index);
    }
    std::sort(order.begin(), order.end(), [&](std::size_t left_index, std::size_t right_index){
        const auto& [left_u, left_v, left_weight] = edges[left_index];
        const auto& [right_u, right_v, right_weight] = edges[right_index];
        if(left_weight != right_weight) return left_weight > right_weight;
        const auto left_edge = std::minmax(left_u, left_v);
        const auto right_edge = std::minmax(right_u, right_v);
        if(left_edge != right_edge) return left_edge < right_edge;
        return left_index < right_index;
    });

    MatchingResult result;
    result.mate.assign(vertex_count, vertex_count);
    for(const std::size_t edge_index : order){
        const auto& [left, right, weight] = edges[edge_index];
        static_cast<void>(weight);
        if(result.mate[left] == vertex_count && result.mate[right] == vertex_count){
            result.mate[left] = right;
            result.mate[right] = left;
            result.edge_indices.push_back(edge_index);
        }
    }
    return result;
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_MATCHING_HPP_INCLUDED
