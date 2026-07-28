#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_CIRCULATION_WITH_LOWER_BOUNDS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_CIRCULATION_WITH_LOWER_BOUNDS_HPP_INCLUDED

#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

#include "dinic.hpp"

struct LowerBoundCirculationEdge{
    int from;
    int to;
    long long lower;
    long long upper;
};

struct LowerBoundCirculationResult{
    std::vector<long long> flow;
};

inline std::optional<LowerBoundCirculationResult>
circulation_with_lower_bounds(
    int vertex_count,
    const std::vector<LowerBoundCirculationEdge>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(circulation_with_lower_bounds)."
        );
    }
    if(vertex_count > std::numeric_limits<int>::max() - 2)[[unlikely]]{
        throw std::overflow_error(
            "circulation_with_lower_bounds graph is too large"
        );
    }
    const int super_source = vertex_count;
    const int super_sink = vertex_count + 1;
    Dinic<long long> flow(vertex_count + 2);
    std::vector<long long> balance(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::vector<int> residual_edge_ids;
    residual_edge_ids.reserve(edges.size());
    for(const auto& edge: edges){
        if(edge.from < 0 || vertex_count <= edge.from
            || edge.to < 0 || vertex_count <= edge.to
            || edge.lower < 0 || edge.upper < edge.lower)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(circulation_with_lower_bounds)."
            );
        }
        residual_edge_ids.push_back(
            flow.add_edge(edge.from, edge.to, edge.upper - edge.lower)
        );
        if(balance[static_cast<std::size_t>(edge.from)]
                < std::numeric_limits<long long>::min() + edge.lower
            || balance[static_cast<std::size_t>(edge.to)]
                > std::numeric_limits<long long>::max() - edge.lower)[[unlikely]]{
            throw std::overflow_error(
                "circulation_with_lower_bounds balance overflows long long"
            );
        }
        balance[static_cast<std::size_t>(edge.from)] -= edge.lower;
        balance[static_cast<std::size_t>(edge.to)] += edge.lower;
    }
    long long required = 0;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        long long value = balance[static_cast<std::size_t>(vertex)];
        if(value > 0){
            flow.add_edge(super_source, vertex, value);
            if(required > std::numeric_limits<long long>::max() - value)[[unlikely]]{
                throw std::overflow_error(
                    "circulation_with_lower_bounds demand overflows long long"
                );
            }
            required += value;
        }else if(value < 0){
            if(value == std::numeric_limits<long long>::min())[[unlikely]]{
                throw std::overflow_error(
                    "circulation_with_lower_bounds demand overflows long long"
                );
            }
            flow.add_edge(vertex, super_sink, -value);
        }
    }
    if(flow.max_flow(super_source, super_sink, required) != required){
        return std::nullopt;
    }
    LowerBoundCirculationResult result;
    result.flow.resize(edges.size());
    for(std::size_t index = 0; index < edges.size(); ++index){
        result.flow[index] = edges[index].lower
            + flow.edges[static_cast<std::size_t>(
                residual_edge_ids[index]
            )].flow;
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_CIRCULATION_WITH_LOWER_BOUNDS_HPP_INCLUDED
