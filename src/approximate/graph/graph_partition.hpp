#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_GRAPH_PARTITION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_GRAPH_PARTITION_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "weighted_undirected_graph.hpp"

namespace approximate::graph {

struct BipartitionResult {
    std::vector<unsigned char> side;
    long double cut_weight = 0.0L;
};

[[nodiscard]] inline long double weighted_cut_weight(
    std::size_t vertex_count,
    const std::vector<WeightedUndirectedEdge>& edges,
    const std::vector<unsigned char>& side
){
    if(side.size() != vertex_count){
        throw std::invalid_argument("partition side vector has the wrong size");
    }
    for(const unsigned char value : side){
        if(value > 1) throw std::invalid_argument("partition sides must be zero or one");
    }
    const detail::WeightedUndirectedGraph graph = detail::make_weighted_undirected_graph(vertex_count, edges);
    long double result = 0.0L;
    for(const WeightedUndirectedEdge& edge : graph.edges){
        if(side[edge.left] != side[edge.right]){
            result = detail::checked_nonnegative_weight_sum(result, edge.weight);
        }
    }
    return result;
}

[[nodiscard]] inline std::vector<unsigned char> balanced_bipartition(std::size_t vertex_count){
    std::vector<unsigned char> side(vertex_count, 1);
    const std::size_t zero_count = (vertex_count + 1) / 2;
    for(std::size_t vertex = 0; vertex < zero_count; ++vertex) side[vertex] = 0;
    return side;
}

namespace detail {

inline void validate_partition(const std::vector<unsigned char>& side, std::size_t vertex_count){
    if(side.size() != vertex_count){
        throw std::invalid_argument("partition side vector has the wrong size");
    }
    for(const unsigned char value : side){
        if(value > 1) throw std::invalid_argument("partition sides must be zero or one");
    }
}

inline void validate_maximum_imbalance(std::size_t vertex_count, std::size_t maximum_imbalance){
    if(maximum_imbalance < vertex_count % 2){
        throw std::invalid_argument("maximum imbalance is infeasible");
    }
}

[[nodiscard]] inline std::size_t partition_imbalance(const std::vector<unsigned char>& side){
    std::size_t zero_count = 0;
    for(const unsigned char value : side) zero_count += value == 0 ? 1U : 0U;
    const std::size_t one_count = side.size() - zero_count;
    return zero_count > one_count ? zero_count - one_count : one_count - zero_count;
}

[[nodiscard]] inline long double normalized_cut_weight(
    const WeightedUndirectedGraph& graph,
    const std::vector<unsigned char>& side
){
    long double result = 0.0L;
    for(const WeightedUndirectedEdge& edge : graph.edges){
        if(side[edge.left] != side[edge.right]) result += edge.weight / graph.total_edge_weight;
    }
    return result;
}

[[nodiscard]] inline BipartitionResult make_bipartition_result(
    const WeightedUndirectedGraph& graph,
    std::vector<unsigned char> side
){
    long double cut_weight = 0.0L;
    for(const WeightedUndirectedEdge& edge : graph.edges){
        if(side[edge.left] != side[edge.right]){
            cut_weight = checked_nonnegative_weight_sum(cut_weight, edge.weight);
        }
    }
    return {std::move(side), cut_weight};
}

inline void rebalance_partition(
    const WeightedUndirectedGraph& graph,
    std::vector<unsigned char>& side,
    std::size_t maximum_imbalance
){
    validate_partition(side, graph.adjacency.size());
    validate_maximum_imbalance(side.size(), maximum_imbalance);
    std::size_t count[2] = {0, 0};
    for(const unsigned char value : side) ++count[value];
    while((count[0] > count[1] ? count[0] - count[1] : count[1] - count[0]) > maximum_imbalance){
        const unsigned char source = count[0] > count[1] ? 0 : 1;
        std::size_t best_vertex = side.size();
        long double best_increase = 0.0L;
        bool found = false;
        for(std::size_t vertex = 0; vertex < side.size(); ++vertex){
            if(side[vertex] != source) continue;
            long double external = 0.0L;
            long double internal = 0.0L;
            for(const WeightedNeighbor& neighbor : graph.adjacency[vertex]){
                if(side[neighbor.vertex] == source){
                    internal += neighbor.normalized_weight;
                }else{
                    external += neighbor.normalized_weight;
                }
            }
            const long double increase = internal - external;
            if(!found || increase < best_increase || (increase == best_increase && vertex < best_vertex)){
                found = true;
                best_increase = increase;
                best_vertex = vertex;
            }
        }
        if(!found) break;
        side[best_vertex] ^= 1U;
        --count[source];
        ++count[source ^ 1U];
    }
}

}  // namespace detail
}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_GRAPH_PARTITION_HPP_INCLUDED
