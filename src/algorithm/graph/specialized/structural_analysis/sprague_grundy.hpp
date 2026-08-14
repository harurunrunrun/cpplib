#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_STRUCTURAL_ANALYSIS_SPRAGUE_GRUNDY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_STRUCTURAL_ANALYSIS_SPRAGUE_GRUNDY_HPP_INCLUDED

#include <cstddef>
#include <queue>
#include <stdexcept>
#include <vector>

#include "../../../../structure/ordered_set/integer/mex_marker.hpp"

inline std::vector<std::size_t> dag_grundy_numbers(
    const std::vector<std::vector<std::size_t>>& graph
){
    const std::size_t size = graph.size();
    std::vector<std::size_t> indegree(size, 0);
    for(const auto& edges: graph){
        for(const std::size_t to: edges){
            if(to >= size) throw std::out_of_range("dag_grundy_numbers: endpoint is out of range");
            ++indegree[to];
        }
    }
    std::queue<std::size_t> queue;
    for(std::size_t vertex = 0; vertex < size; vertex++){
        if(indegree[vertex] == 0) queue.push(vertex);
    }
    std::vector<std::size_t> order;
    order.reserve(size);
    while(!queue.empty()){
        const std::size_t vertex = queue.front();
        queue.pop();
        order.push_back(vertex);
        for(const std::size_t to: graph[vertex]){
            if(--indegree[to] == 0) queue.push(to);
        }
    }
    if(order.size() != size) throw std::invalid_argument("dag_grundy_numbers: graph is not acyclic");

    std::vector<std::size_t> grundy(size, 0);
    MexMarker marker(size + 1);
    for(auto iterator = order.rbegin(); iterator != order.rend(); ++iterator){
        marker.clear();
        for(const std::size_t to: graph[*iterator]) marker.mark(grundy[to]);
        grundy[*iterator] = marker.mex();
    }
    return grundy;
}

inline std::size_t normal_play_nim_sum(
    const std::vector<std::size_t>& grundy,
    const std::vector<std::size_t>& positions
){
    std::size_t answer = 0;
    for(const std::size_t position: positions){
        if(position >= grundy.size()) throw std::out_of_range("normal_play_nim_sum: position is out of range");
        answer ^= grundy[position];
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_STRUCTURAL_ANALYSIS_SPRAGUE_GRUNDY_HPP_INCLUDED
