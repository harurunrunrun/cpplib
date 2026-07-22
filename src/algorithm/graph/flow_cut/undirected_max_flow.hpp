#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_UNDIRECTED_MAX_FLOW_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_UNDIRECTED_MAX_FLOW_HPP_INCLUDED

#include <stdexcept>
#include <type_traits>
#include <vector>

#include "../../matching/flow/dinic.hpp"

template<class T>
struct UndirectedFlowEdge{
    int from;
    int to;
    T capacity;
};

template<class T>
T undirected_max_flow(
    int n,
    const std::vector<UndirectedFlowEdge<T>>& edges,
    int source,
    int sink
){
    static_assert(std::is_arithmetic_v<T>);
    if(n < 0 || source < 0 || n <= source || sink < 0 || n <= sink)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (undirected_max_flow)."
        );
    }
    Dinic<T> flow(n);
    for(const auto& edge: edges){
        if(edge.from < 0 || n <= edge.from || edge.to < 0 || n <= edge.to ||
           edge.capacity < T(0))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (undirected_max_flow)."
            );
        }
        if(edge.from == edge.to) continue;
        flow.add_edge(edge.from, edge.to, edge.capacity);
        flow.add_edge(edge.to, edge.from, edge.capacity);
    }
    return flow.max_flow(source, sink);
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_UNDIRECTED_MAX_FLOW_HPP_INCLUDED
