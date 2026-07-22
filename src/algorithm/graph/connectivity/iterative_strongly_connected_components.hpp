#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_ITERATIVE_STRONGLY_CONNECTED_COMPONENTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_ITERATIVE_STRONGLY_CONNECTED_COMPONENTS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "strongly_connected_components.hpp"

inline StronglyConnectedComponentsResult
iterative_strongly_connected_components(
    const std::vector<std::vector<int>>& graph
){
    if(graph.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error("iterative SCC graph is too large");
    }
    const int size = static_cast<int>(graph.size());
    std::vector<std::vector<int>> reverse_graph(
        static_cast<std::size_t>(size)
    );
    for(int vertex = 0; vertex < size; ++vertex){
        for(const int to: graph[static_cast<std::size_t>(vertex)]){
            if(to < 0 || size <= to)[[unlikely]]{
                throw std::out_of_range(
                    "iterative SCC edge endpoint is out of range"
                );
            }
            reverse_graph[static_cast<std::size_t>(to)].push_back(vertex);
        }
    }

    std::vector<unsigned char> seen(static_cast<std::size_t>(size));
    std::vector<int> order;
    order.reserve(static_cast<std::size_t>(size));
    std::vector<std::pair<int, std::size_t>> stack;
    stack.reserve(static_cast<std::size_t>(size));
    for(int start = 0; start < size; ++start){
        if(seen[static_cast<std::size_t>(start)] != 0) continue;
        seen[static_cast<std::size_t>(start)] = 1;
        stack.emplace_back(start, 0);
        while(!stack.empty()){
            auto& [vertex, next_edge] = stack.back();
            const auto& edges = graph[static_cast<std::size_t>(vertex)];
            if(next_edge == edges.size()){
                order.push_back(vertex);
                stack.pop_back();
                continue;
            }
            const int to = edges[next_edge++];
            if(seen[static_cast<std::size_t>(to)] == 0){
                seen[static_cast<std::size_t>(to)] = 1;
                stack.emplace_back(to, 0);
            }
        }
    }

    StronglyConnectedComponentsResult result;
    result.id.assign(static_cast<std::size_t>(size), -1);
    std::vector<int> vertices;
    vertices.reserve(static_cast<std::size_t>(size));
    for(auto iterator = order.rbegin(); iterator != order.rend(); ++iterator){
        const int start = *iterator;
        if(result.id[static_cast<std::size_t>(start)] != -1) continue;
        const int component = static_cast<int>(result.groups.size());
        result.groups.emplace_back();
        vertices.push_back(start);
        result.id[static_cast<std::size_t>(start)] = component;
        while(!vertices.empty()){
            const int vertex = vertices.back();
            vertices.pop_back();
            result.groups[static_cast<std::size_t>(component)].push_back(vertex);
            for(const int to:
                reverse_graph[static_cast<std::size_t>(vertex)]){
                if(result.id[static_cast<std::size_t>(to)] == -1){
                    result.id[static_cast<std::size_t>(to)] = component;
                    vertices.push_back(to);
                }
            }
        }
    }
    result.count = static_cast<int>(result.groups.size());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_ITERATIVE_STRONGLY_CONNECTED_COMPONENTS_HPP_INCLUDED
