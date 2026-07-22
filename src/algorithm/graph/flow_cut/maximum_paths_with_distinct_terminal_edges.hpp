#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_MAXIMUM_PATHS_WITH_DISTINCT_TERMINAL_EDGES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_MAXIMUM_PATHS_WITH_DISTINCT_TERMINAL_EDGES_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../matching/dinic.hpp"

inline long long maximum_paths_with_distinct_terminal_edges(
    int vertex_count,
    const std::vector<std::pair<int, int>>& directed_edges,
    int source,
    int sink
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::invalid_argument(
            "maximum_paths_with_distinct_terminal_edges "
            "negative vertex count"
        );
    }
    if(source < 0 || vertex_count <= source ||
       sink < 0 || vertex_count <= sink)[[unlikely]]{
        throw std::out_of_range(
            "maximum_paths_with_distinct_terminal_edges "
            "terminal is out of range"
        );
    }
    if(source == sink)[[unlikely]]{
        throw std::invalid_argument(
            "maximum_paths_with_distinct_terminal_edges "
            "requires distinct terminals"
        );
    }
    if(directed_edges.size() >= static_cast<std::size_t>(
        std::numeric_limits<long long>::max()
    ))[[unlikely]]{
        throw std::length_error(
            "maximum_paths_with_distinct_terminal_edges "
            "has too many edges"
        );
    }

    const long long internal_capacity =
        static_cast<long long>(directed_edges.size()) + 1;
    Dinic<long long> flow(vertex_count);
    for(const auto [from, to]: directed_edges){
        if(from < 0 || vertex_count <= from ||
           to < 0 || vertex_count <= to)[[unlikely]]{
            throw std::out_of_range(
                "maximum_paths_with_distinct_terminal_edges "
                "edge is out of range"
            );
        }
        if(from == to) continue;
        const long long capacity =
            from == source || to == sink ? 1 : internal_capacity;
        flow.add_edge(from, to, capacity);
    }
    return flow.max_flow(source, sink);
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_MAXIMUM_PATHS_WITH_DISTINCT_TERMINAL_EDGES_HPP_INCLUDED
