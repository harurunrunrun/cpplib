#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_MAX_CUT_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_MAX_CUT_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "detail/undirected_graph.hpp"

namespace approximate::graph {

struct MaxCutResult {
    std::vector<unsigned char> side;
    std::size_t cut_edges = 0;
};

[[nodiscard]] inline std::size_t cut_edge_count(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    const std::vector<unsigned char>& side
){
    if(side.size() != vertex_count){
        throw std::invalid_argument("cut side vector has the wrong size");
    }
    std::size_t result = 0;
    for(const auto [left, right] : edges){
        detail::validate_endpoint(vertex_count, left);
        detail::validate_endpoint(vertex_count, right);
        if(side[left] > 1 || side[right] > 1){
            throw std::invalid_argument("cut sides must be zero or one");
        }
        if(side[left] != side[right]) ++result;
    }
    return result;
}

template<class UniformRandomBitGenerator>
[[nodiscard]] MaxCutResult random_cut(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    UniformRandomBitGenerator& generator,
    double probability_of_one = 0.5
){
    if(!std::isfinite(probability_of_one) || probability_of_one < 0.0 || probability_of_one > 1.0){
        throw std::invalid_argument("cut probability must be in [0, 1]");
    }
    for(const auto [left, right] : edges){
        detail::validate_endpoint(vertex_count, left);
        detail::validate_endpoint(vertex_count, right);
    }
    std::bernoulli_distribution choose_one(probability_of_one);
    MaxCutResult result;
    result.side.resize(vertex_count);
    for(unsigned char& side : result.side){
        side = static_cast<unsigned char>(choose_one(generator));
    }
    result.cut_edges = cut_edge_count(vertex_count, edges, result.side);
    return result;
}

[[nodiscard]] inline MaxCutResult greedy_max_cut(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    std::vector<std::vector<std::size_t>> previous_neighbors(vertex_count);
    for(const auto [left, right] : edges){
        detail::validate_endpoint(vertex_count, left);
        detail::validate_endpoint(vertex_count, right);
        if(left == right) continue;
        if(left < right){
            previous_neighbors[right].push_back(left);
        }else{
            previous_neighbors[left].push_back(right);
        }
    }
    MaxCutResult result;
    result.side.assign(vertex_count, 0);
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        std::size_t cut_if_zero = 0;
        std::size_t cut_if_one = 0;
        for(const std::size_t neighbor : previous_neighbors[vertex]){
            if(result.side[neighbor] == 0){
                ++cut_if_one;
            }else{
                ++cut_if_zero;
            }
        }
        if(cut_if_one > cut_if_zero) result.side[vertex] = 1;
    }
    result.cut_edges = cut_edge_count(vertex_count, edges, result.side);
    return result;
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_MAX_CUT_HPP_INCLUDED
