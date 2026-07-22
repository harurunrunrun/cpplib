#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_CHARACTER_LABELED_UNDIRECTED_MAX_FLOW_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_CHARACTER_LABELED_UNDIRECTED_MAX_FLOW_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "undirected_max_flow.hpp"

template<class T>
struct CharacterLabeledUndirectedFlowEdge{
    char from;
    char to;
    T capacity;
};

template<class T>
T character_labeled_undirected_max_flow(
    const std::vector<CharacterLabeledUndirectedFlowEdge<T>>& edges,
    char source_label,
    char sink_label
){
    static_assert(std::is_arithmetic_v<T>);
    if(source_label == sink_label)[[unlikely]]{
        throw std::invalid_argument(
            "character_labeled_undirected_max_flow "
            "requires distinct terminals"
        );
    }

    std::array<int, 256> label_id;
    label_id.fill(-1);
    int vertex_count = 0;
    const auto assign_id = [&](char label){
        const std::size_t index =
            static_cast<unsigned char>(label);
        if(label_id[index] == -1){
            label_id[index] = vertex_count++;
        }
        return label_id[index];
    };

    const int source = assign_id(source_label);
    const int sink = assign_id(sink_label);
    std::vector<UndirectedFlowEdge<T>> indexed_edges;
    indexed_edges.reserve(edges.size());
    for(const auto& edge: edges){
        indexed_edges.push_back({
            assign_id(edge.from),
            assign_id(edge.to),
            edge.capacity,
        });
    }
    return undirected_max_flow(
        vertex_count, indexed_edges, source, sink
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_CHARACTER_LABELED_UNDIRECTED_MAX_FLOW_HPP_INCLUDED
