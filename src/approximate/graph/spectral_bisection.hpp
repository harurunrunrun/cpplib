#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_SPECTRAL_BISECTION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_SPECTRAL_BISECTION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include "graph_partition.hpp"

namespace approximate::graph {
namespace detail {

inline void center_and_normalize(std::vector<long double>& values){
    long double mean = 0.0L;
    for(std::size_t index = 0; index < values.size(); ++index){
        mean += (values[index] - mean) / static_cast<long double>(index + 1);
    }
    long double norm = 0.0L;
    for(long double& value : values){
        value -= mean;
        norm = std::hypot(norm, value);
    }
    if(norm == 0.0L) return;
    for(long double& value : values) value /= norm;
}

}  // namespace detail

[[nodiscard]] inline BipartitionResult spectral_bisection(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    std::size_t maximum_iterations = 300,
    long double tolerance = 1.0e-12L
){
    if(!std::isfinite(tolerance) || tolerance < 0.0L){
        throw std::invalid_argument("spectral tolerance must be finite and nonnegative");
    }
    const detail::WeightedUndirectedGraph graph = detail::make_weighted_undirected_graph(vertex_count, edges);
    if(vertex_count < 2 || graph.total_edge_weight == 0.0L || maximum_iterations == 0){
        return detail::make_bipartition_result(graph, balanced_bipartition(vertex_count));
    }
    const long double maximum_degree = *std::max_element(
        graph.normalized_degree.begin(), graph.normalized_degree.end()
    );
    if(maximum_degree == 0.0L){
        return detail::make_bipartition_result(graph, balanced_bipartition(vertex_count));
    }

    std::vector<long double> vector(vertex_count);
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        vector[vertex] = std::sin(1.6180339887498948482L * static_cast<long double>(vertex + 1));
    }
    detail::center_and_normalize(vector);
    long double norm = 0.0L;
    for(const long double value : vector) norm = std::hypot(norm, value);
    if(norm == 0.0L){
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            vector[vertex] = static_cast<long double>(vertex);
        }
        detail::center_and_normalize(vector);
    }

    const long double step = 0.5L / maximum_degree;
    std::vector<long double> next(vertex_count);
    for(std::size_t iteration = 0; iteration < maximum_iterations; ++iteration){
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            long double neighbor_sum = 0.0L;
            for(const detail::WeightedNeighbor& neighbor : graph.adjacency[vertex]){
                neighbor_sum += neighbor.normalized_weight * vector[neighbor.vertex];
            }
            next[vertex] = vector[vertex] - step * (
                graph.normalized_degree[vertex] * vector[vertex] - neighbor_sum
            );
        }
        detail::center_and_normalize(next);
        long double difference = 0.0L;
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            difference = std::max(difference, std::abs(next[vertex] - vector[vertex]));
        }
        vector.swap(next);
        if(difference <= tolerance) break;
    }

    std::vector<std::size_t> order(vertex_count);
    std::iota(order.begin(), order.end(), 0);
    std::stable_sort(order.begin(), order.end(), [&vector](std::size_t left, std::size_t right){
        if(vector[left] != vector[right]) return vector[left] < vector[right];
        return left < right;
    });
    std::vector<unsigned char> side(vertex_count, 1);
    const std::size_t zero_count = (vertex_count + 1) / 2;
    for(std::size_t index = 0; index < zero_count; ++index) side[order[index]] = 0;
    return detail::make_bipartition_result(graph, std::move(side));
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_SPECTRAL_BISECTION_HPP_INCLUDED
