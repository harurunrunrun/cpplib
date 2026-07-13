#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <numeric>
#include <queue>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "detail/undirected_graph.hpp"
#include "independent_set.hpp"

namespace approximate::graph {

struct ColoringResult {
    std::vector<std::size_t> colors;
    std::size_t color_count = 0;
};

[[nodiscard]] inline ColoringResult greedy_coloring(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    std::vector<std::size_t> order = {}
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    if(std::find(graph.self_loop.begin(), graph.self_loop.end(), true) != graph.self_loop.end()){
        throw std::invalid_argument("a graph with a self-loop has no proper coloring");
    }
    if(order.empty()){
        order.resize(vertex_count);
        std::iota(order.begin(), order.end(), std::size_t{0});
    }else{
        detail::validate_vertex_order(vertex_count, order);
    }
    ColoringResult result;
    result.colors.assign(vertex_count, vertex_count);
    std::vector<std::size_t> forbidden(vertex_count, vertex_count);
    for(const std::size_t vertex : order){
        for(const std::size_t neighbor : graph.adjacency[vertex]){
            if(result.colors[neighbor] != vertex_count){
                forbidden[result.colors[neighbor]] = vertex;
            }
        }
        std::size_t color = 0;
        while(color < vertex_count && forbidden[color] == vertex) ++color;
        result.colors[vertex] = color;
        result.color_count = std::max(result.color_count, color + 1);
    }
    return result;
}

[[nodiscard]] inline std::vector<std::size_t> largest_first_order(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    std::vector<std::size_t> order(vertex_count);
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::sort(order.begin(), order.end(), [&](std::size_t left, std::size_t right){
        if(graph.adjacency[left].size() != graph.adjacency[right].size()){
            return graph.adjacency[left].size() > graph.adjacency[right].size();
        }
        return left < right;
    });
    return order;
}

[[nodiscard]] inline ColoringResult largest_first_coloring(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    return greedy_coloring(vertex_count, edges, largest_first_order(vertex_count, edges));
}

[[nodiscard]] inline ColoringResult welsh_powell_coloring(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    return largest_first_coloring(vertex_count, edges);
}

[[nodiscard]] inline std::vector<std::size_t> smallest_last_order(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    std::vector<std::size_t> degree(vertex_count);
    std::vector<bool> active(vertex_count, true);
    using Entry = std::pair<std::size_t, std::size_t>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> heap;
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        degree[vertex] = graph.adjacency[vertex].size();
        heap.emplace(degree[vertex], vertex);
    }
    std::vector<std::size_t> removal;
    removal.reserve(vertex_count);
    while(!heap.empty()){
        const auto [entry_degree, vertex] = heap.top();
        heap.pop();
        if(!active[vertex] || entry_degree != degree[vertex]) continue;
        active[vertex] = false;
        removal.push_back(vertex);
        for(const std::size_t neighbor : graph.adjacency[vertex]){
            if(active[neighbor]){
                --degree[neighbor];
                heap.emplace(degree[neighbor], neighbor);
            }
        }
    }
    std::reverse(removal.begin(), removal.end());
    return removal;
}

[[nodiscard]] inline ColoringResult smallest_last_coloring(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    return greedy_coloring(vertex_count, edges, smallest_last_order(vertex_count, edges));
}

[[nodiscard]] inline ColoringResult dsatur_coloring(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    const detail::UndirectedGraph graph = detail::make_undirected_graph(vertex_count, edges);
    if(std::find(graph.self_loop.begin(), graph.self_loop.end(), true) != graph.self_loop.end()){
        throw std::invalid_argument("a graph with a self-loop has no proper coloring");
    }
    ColoringResult result;
    result.colors.assign(vertex_count, vertex_count);
    std::vector<std::set<std::size_t>> neighbor_colors(vertex_count);
    std::vector<std::size_t> forbidden(vertex_count, vertex_count);
    for(std::size_t step = 0; step < vertex_count; ++step){
        std::size_t selected = vertex_count;
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            if(result.colors[vertex] != vertex_count) continue;
            if(selected == vertex_count ||
               neighbor_colors[vertex].size() > neighbor_colors[selected].size() ||
               (neighbor_colors[vertex].size() == neighbor_colors[selected].size() &&
                graph.adjacency[vertex].size() > graph.adjacency[selected].size()) ||
               (neighbor_colors[vertex].size() == neighbor_colors[selected].size() &&
                graph.adjacency[vertex].size() == graph.adjacency[selected].size() &&
                vertex < selected)){
                selected = vertex;
            }
        }
        for(const std::size_t neighbor : graph.adjacency[selected]){
            if(result.colors[neighbor] != vertex_count){
                forbidden[result.colors[neighbor]] = selected;
            }
        }
        std::size_t color = 0;
        while(color < vertex_count && forbidden[color] == selected) ++color;
        result.colors[selected] = color;
        result.color_count = std::max(result.color_count, color + 1);
        for(const std::size_t neighbor : graph.adjacency[selected]){
            if(result.colors[neighbor] == vertex_count){
                neighbor_colors[neighbor].insert(color);
            }
        }
    }
    return result;
}

}  // namespace approximate::graph
