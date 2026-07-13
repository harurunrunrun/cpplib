#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace approximate::graph {

struct WeightedUndirectedEdge {
    std::size_t left;
    std::size_t right;
    long double weight;
};

namespace detail {

struct WeightedNeighbor {
    std::size_t vertex;
    long double weight;
    long double normalized_weight;
};

struct WeightedUndirectedGraph {
    std::vector<WeightedUndirectedEdge> edges;
    std::vector<std::vector<WeightedNeighbor>> adjacency;
    std::vector<long double> degree;
    std::vector<long double> normalized_degree;
    long double total_edge_weight = 0.0L;
};

inline void validate_weighted_endpoint(std::size_t vertex_count, std::size_t vertex){
    if(vertex >= vertex_count){
        throw std::out_of_range("edge endpoint is outside the graph");
    }
}

[[nodiscard]] inline long double checked_nonnegative_weight_sum(long double left, long double right){
    if(left > std::numeric_limits<long double>::max() - right){
        throw std::overflow_error("the sum of edge weights is not representable");
    }
    const long double result = left + right;
    if(!std::isfinite(result)){
        throw std::overflow_error("the sum of edge weights is not representable");
    }
    return result;
}

[[nodiscard]] inline WeightedUndirectedGraph make_weighted_undirected_graph(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& input_edges
){
    std::vector<WeightedUndirectedEdge> sorted_edges;
    sorted_edges.reserve(input_edges.size());
    for(const WeightedUndirectedEdge& edge : input_edges){
        validate_weighted_endpoint(vertex_count, edge.left);
        validate_weighted_endpoint(vertex_count, edge.right);
        if(!std::isfinite(edge.weight) || edge.weight < 0.0L){
            throw std::invalid_argument("edge weights must be finite and nonnegative");
        }
        if(edge.left == edge.right || edge.weight == 0.0L) continue;
        sorted_edges.push_back({std::min(edge.left, edge.right), std::max(edge.left, edge.right), edge.weight});
    }
    std::sort(sorted_edges.begin(), sorted_edges.end(), [](const auto& left, const auto& right){
        if(left.left != right.left) return left.left < right.left;
        return left.right < right.right;
    });

    WeightedUndirectedGraph graph;
    graph.adjacency.resize(vertex_count);
    graph.degree.assign(vertex_count, 0.0L);
    for(const WeightedUndirectedEdge& edge : sorted_edges){
        if(!graph.edges.empty() && graph.edges.back().left == edge.left && graph.edges.back().right == edge.right){
            graph.edges.back().weight = checked_nonnegative_weight_sum(graph.edges.back().weight, edge.weight);
        }else{
            graph.edges.push_back(edge);
        }
    }
    for(const WeightedUndirectedEdge& edge : graph.edges){
        graph.total_edge_weight = checked_nonnegative_weight_sum(graph.total_edge_weight, edge.weight);
        graph.degree[edge.left] = checked_nonnegative_weight_sum(graph.degree[edge.left], edge.weight);
        graph.degree[edge.right] = checked_nonnegative_weight_sum(graph.degree[edge.right], edge.weight);
    }

    graph.normalized_degree.assign(vertex_count, 0.0L);
    if(graph.total_edge_weight == 0.0L) return graph;
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        graph.normalized_degree[vertex] = graph.degree[vertex] / graph.total_edge_weight;
    }
    for(const WeightedUndirectedEdge& edge : graph.edges){
        const long double normalized_weight = edge.weight / graph.total_edge_weight;
        graph.adjacency[edge.left].push_back({edge.right, edge.weight, normalized_weight});
        graph.adjacency[edge.right].push_back({edge.left, edge.weight, normalized_weight});
    }
    return graph;
}

[[nodiscard]] inline long double normalized_pair_weight(
    const WeightedUndirectedGraph& graph,
    std::size_t left,
    std::size_t right
){
    const auto& neighbors = graph.adjacency[left];
    const auto iterator = std::lower_bound(
        neighbors.begin(), neighbors.end(), right,
        [](const WeightedNeighbor& neighbor, std::size_t vertex){ return neighbor.vertex < vertex; }
    );
    return iterator != neighbors.end() && iterator->vertex == right ? iterator->normalized_weight : 0.0L;
}

}  // namespace detail
}  // namespace approximate::graph
