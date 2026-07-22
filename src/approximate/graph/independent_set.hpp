#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_INDEPENDENT_SET_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_INDEPENDENT_SET_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "detail/undirected_graph.hpp"

namespace approximate::graph {

namespace detail {

inline void validate_vertex_order(
    std::size_t vertex_count,
    const std::vector<std::size_t>& order
){
    if(order.size() != vertex_count){
        throw std::invalid_argument("vertex order must be a permutation");
    }
    std::vector<bool> used(vertex_count, false);
    for(const std::size_t vertex : order){
        if(vertex >= vertex_count || used[vertex]){
            throw std::invalid_argument("vertex order must be a permutation");
        }
        used[vertex] = true;
    }
}

}  // namespace detail

[[nodiscard]] inline std::vector<std::size_t> maximal_independent_set(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    std::vector<std::size_t> order = {}
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    if(order.empty()){
        order.resize(vertex_count);
        std::iota(order.begin(), order.end(), std::size_t{0});
    }else{
        detail::validate_vertex_order(vertex_count, order);
    }
    std::vector<bool> blocked(vertex_count, false);
    std::vector<std::size_t> result;
    for(const std::size_t vertex : order){
        if(blocked[vertex] || graph.self_loop[vertex]) continue;
        result.push_back(vertex);
        blocked[vertex] = true;
        for(const std::size_t neighbor : graph.adjacency[vertex]) blocked[neighbor] = true;
    }
    return result;
}

[[nodiscard]] inline std::vector<std::size_t> greedy_minimum_degree_independent_set(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    std::vector<bool> active(vertex_count, true);
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        if(graph.self_loop[vertex]) active[vertex] = false;
    }
    std::vector<std::size_t> degree(vertex_count, 0);
    using Entry = std::pair<std::size_t, std::size_t>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> heap;
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        if(!active[vertex]) continue;
        for(const std::size_t neighbor : graph.adjacency[vertex]){
            if(active[neighbor]) ++degree[vertex];
        }
        heap.emplace(degree[vertex], vertex);
    }

    const auto remove_vertex = [&](std::size_t vertex){
        if(!active[vertex]) return;
        active[vertex] = false;
        for(const std::size_t neighbor : graph.adjacency[vertex]){
            if(active[neighbor]){
                --degree[neighbor];
                heap.emplace(degree[neighbor], neighbor);
            }
        }
    };

    std::vector<std::size_t> result;
    while(!heap.empty()){
        const auto [entry_degree, vertex] = heap.top();
        heap.pop();
        if(!active[vertex] || entry_degree != degree[vertex]) continue;
        result.push_back(vertex);
        const std::vector<std::size_t> neighbors = graph.adjacency[vertex];
        remove_vertex(vertex);
        for(const std::size_t neighbor : neighbors) remove_vertex(neighbor);
    }
    return result;
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_INDEPENDENT_SET_HPP_INCLUDED
