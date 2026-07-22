#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_MAXIMUM_INDEPENDENT_SET_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_MAXIMUM_INDEPENDENT_SET_HPP_INCLUDED

#include <stdexcept>
#include <vector>
#include "maximum_clique.hpp"

std::vector<int> maximum_independent_set(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    std::vector<std::vector<char>> adjacent(static_cast<std::size_t>(n), std::vector<char>(static_cast<std::size_t>(n), 0));
    for(int v = 0; v < n; v++){
        for(int to: graph[static_cast<std::size_t>(v)]){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (maximum_independent_set).");
            }
            if(to != v) adjacent[static_cast<std::size_t>(v)][static_cast<std::size_t>(to)] = 1;
        }
    }
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(adjacent[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] !=
               adjacent[static_cast<std::size_t>(j)][static_cast<std::size_t>(i)])[[unlikely]]{
                throw std::runtime_error("library assertion fault: graph must be undirected (maximum_independent_set).");
            }
        }
    }

    std::vector<std::vector<int>> complement(static_cast<std::size_t>(n));
    for(int i = 0; i < n; i++){
        for(int j = i + 1; j < n; j++){
            if(!adjacent[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)]){
                complement[static_cast<std::size_t>(i)].push_back(j);
                complement[static_cast<std::size_t>(j)].push_back(i);
            }
        }
    }
    return maximum_clique(complement);
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_MAXIMUM_INDEPENDENT_SET_HPP_INCLUDED
