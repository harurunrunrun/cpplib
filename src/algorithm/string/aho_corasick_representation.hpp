#ifndef CPPLIB_SRC_ALGORITHM_STRING_AHO_CORASICK_REPRESENTATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_AHO_CORASICK_REPRESENTATION_HPP_INCLUDED

#include <memory>
#include <string>
#include <vector>

#include "aho_corasick.hpp"

struct AhoCorasickRepresentation{
    std::vector<int> parent;
    std::vector<int> failure_link;
    std::vector<int> terminal_state;

    int node_count() const{
        return static_cast<int>(parent.size());
    }
};

template<int ALPHABET, int MAX_NODES, char OFFSET = 'a'>
AhoCorasickRepresentation aho_corasick_representation(
    const std::vector<std::string>& patterns
){
    auto automaton = std::make_unique<
        AhoCorasick<ALPHABET, MAX_NODES, OFFSET>
    >();
    AhoCorasickRepresentation result;
    result.terminal_state.reserve(patterns.size());
    for(const std::string& pattern: patterns){
        result.terminal_state.push_back(automaton->add(pattern));
    }
    automaton->build();

    const int nodes = automaton->node_count();
    result.parent.resize(static_cast<std::size_t>(nodes));
    result.failure_link.resize(static_cast<std::size_t>(nodes));
    for(int node = 0; node < nodes; ++node){
        result.parent[static_cast<std::size_t>(node)] = automaton->parent(node);
        result.failure_link[static_cast<std::size_t>(node)] =
            automaton->failure_link(node);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_AHO_CORASICK_REPRESENTATION_HPP_INCLUDED
