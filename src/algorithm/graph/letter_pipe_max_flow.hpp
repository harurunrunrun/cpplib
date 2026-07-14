#pragma once

#include <stdexcept>
#include <type_traits>
#include <vector>

#include "undirected_max_flow.hpp"

template<class T>
struct LetterPipe{
    char from;
    char to;
    T capacity;
};

namespace letter_pipe_max_flow_internal{

inline int index(char name){
    if('A' <= name && name <= 'Z') return name - 'A';
    if('a' <= name && name <= 'z') return 26 + name - 'a';
    throw std::runtime_error(
        "library assertion fault: invalid name (letter_pipe_max_flow)."
    );
}

} // namespace letter_pipe_max_flow_internal

template<class T>
T letter_pipe_max_flow(const std::vector<LetterPipe<T>>& pipes){
    static_assert(std::is_arithmetic_v<T>);
    std::vector<UndirectedFlowEdge<T>> edges;
    edges.reserve(pipes.size());
    for(const auto& pipe: pipes){
        edges.push_back({
            letter_pipe_max_flow_internal::index(pipe.from),
            letter_pipe_max_flow_internal::index(pipe.to),
            pipe.capacity
        });
    }
    return undirected_max_flow<T>(52, edges, 0, 25);
}
