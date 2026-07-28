#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_MINIMUM_VERTEX_CUT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_MINIMUM_VERTEX_CUT_HPP_INCLUDED

#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../matching/flow/dinic.hpp"

struct MinimumVertexCutResult{
    long long capacity;
    std::vector<int> vertices;
};

namespace minimum_vertex_cut_internal{

inline std::optional<MinimumVertexCutResult> solve(
    const std::vector<long long>& vertex_capacity,
    const std::vector<std::pair<int, int>>& directed_edges,
    int source,
    int sink
){
    const int vertex_count = static_cast<int>(vertex_capacity.size());
    if(source < 0 || vertex_count <= source
        || sink < 0 || vertex_count <= sink)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (minimum_vertex_cut)."
        );
    }
    if(source == sink) return MinimumVertexCutResult{0, {}};
    __int128 sum = 0;
    for(long long capacity: vertex_capacity){
        if(capacity < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative capacity "
                "(minimum_vertex_cut)."
            );
        }
        sum += static_cast<__int128>(capacity);
    }
    if(sum >= std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "minimum_vertex_cut capacity sum overflows long long"
        );
    }
    if(vertex_count > std::numeric_limits<int>::max() / 2)[[unlikely]]{
        throw std::overflow_error("minimum_vertex_cut graph is too large");
    }
    const long long infinity = static_cast<long long>(sum) + 1;
    Dinic<long long> flow(2 * vertex_count);
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        long long capacity = vertex == source || vertex == sink
            ? infinity
            : vertex_capacity[static_cast<std::size_t>(vertex)];
        flow.add_edge(2 * vertex, 2 * vertex + 1, capacity);
    }
    for(auto [from, to]: directed_edges){
        if(from < 0 || vertex_count <= from
            || to < 0 || vertex_count <= to)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(minimum_vertex_cut)."
            );
        }
        flow.add_edge(2 * from + 1, 2 * to, infinity);
    }
    long long value = flow.max_flow(
        2 * source + 1, 2 * sink, infinity
    );
    if(value >= infinity) return std::nullopt;
    std::vector<char> reachable = flow.min_cut(2 * source + 1);
    std::vector<int> vertices;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(vertex != source && vertex != sink
            && reachable[static_cast<std::size_t>(2 * vertex)] != 0
            && reachable[static_cast<std::size_t>(2 * vertex + 1)] == 0){
            vertices.push_back(vertex);
        }
    }
    return MinimumVertexCutResult{value, std::move(vertices)};
}

} // namespace minimum_vertex_cut_internal

inline std::optional<MinimumVertexCutResult> directed_minimum_vertex_cut(
    const std::vector<long long>& vertex_capacity,
    const std::vector<std::pair<int, int>>& directed_edges,
    int source,
    int sink
){
    return minimum_vertex_cut_internal::solve(
        vertex_capacity, directed_edges, source, sink
    );
}

inline std::optional<MinimumVertexCutResult> undirected_minimum_vertex_cut(
    const std::vector<long long>& vertex_capacity,
    const std::vector<std::pair<int, int>>& undirected_edges,
    int source,
    int sink
){
    std::vector<std::pair<int, int>> directed_edges;
    directed_edges.reserve(undirected_edges.size() * 2);
    for(auto [from, to]: undirected_edges){
        directed_edges.emplace_back(from, to);
        if(from != to) directed_edges.emplace_back(to, from);
    }
    return minimum_vertex_cut_internal::solve(
        vertex_capacity, directed_edges, source, sink
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_MINIMUM_VERTEX_CUT_HPP_INCLUDED
