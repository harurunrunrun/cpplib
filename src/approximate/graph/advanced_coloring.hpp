#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_ADVANCED_COLORING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_ADVANCED_COLORING_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <set>
#include <utility>
#include <vector>

#include "coloring.hpp"

namespace approximate::graph {

/**
 * Returns the dynamic saturation-degree order used by DSATUR.
 *
 * Ties are resolved by static degree and then vertex index.  The colors assigned
 * while constructing the order are the smallest available colors.
 *
 * Complexity: O(V^2 + E log V) time and O(V + E) space.
 */
[[nodiscard]] inline std::vector<std::size_t> saturation_degree_ordering(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges
){
    const detail::UndirectedGraph graph =
        detail::make_undirected_graph(vertex_count, edges);
    detail::require_colorable_simple_graph(graph);

    std::vector<std::size_t> order;
    order.reserve(vertex_count);
    std::vector<std::size_t> colors(vertex_count, vertex_count);
    std::vector<std::set<std::size_t>> neighbor_colors(vertex_count);
    std::vector<std::size_t> forbidden(vertex_count, vertex_count);
    for(std::size_t step = 0; step < vertex_count; ++step){
        std::size_t selected = vertex_count;
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            if(colors[vertex] != vertex_count) continue;
            if(selected == vertex_count
               || neighbor_colors[vertex].size()
                    > neighbor_colors[selected].size()
               || (neighbor_colors[vertex].size()
                        == neighbor_colors[selected].size()
                   && graph.adjacency[vertex].size()
                        > graph.adjacency[selected].size())
               || (neighbor_colors[vertex].size()
                        == neighbor_colors[selected].size()
                   && graph.adjacency[vertex].size()
                        == graph.adjacency[selected].size()
                   && vertex < selected)){
                selected = vertex;
            }
        }
        for(const std::size_t neighbor : graph.adjacency[selected]){
            if(colors[neighbor] != vertex_count){
                forbidden[colors[neighbor]] = selected;
            }
        }
        std::size_t color = 0;
        while(color < vertex_count && forbidden[color] == selected) ++color;
        colors[selected] = color;
        order.push_back(selected);
        for(const std::size_t neighbor : graph.adjacency[selected]){
            if(colors[neighbor] == vertex_count){
                neighbor_colors[neighbor].insert(color);
            }
        }
    }
    return order;
}

/**
 * Colors one greedy maximal independent set at a time.
 *
 * `priority` is a vertex permutation.  If omitted, increasing vertex order is
 * used.  Every color class is independent and maximal in the subgraph induced
 * by the vertices that were uncolored at the beginning of that class.
 *
 * Complexity: O(C(V + E)) time and O(V + E) space, where C is the number of
 * produced colors.
 */
[[nodiscard]] inline ColoringResult independent_set_coloring(
    std::size_t vertex_count,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    std::vector<std::size_t> priority = {}
){
    const detail::UndirectedGraph graph =
        detail::make_undirected_graph(vertex_count, edges);
    detail::require_colorable_simple_graph(graph);
    if(priority.empty()){
        priority.resize(vertex_count);
        std::iota(priority.begin(), priority.end(), std::size_t{0});
    }else{
        detail::validate_vertex_order(vertex_count, priority);
    }

    ColoringResult result;
    result.colors.assign(vertex_count, vertex_count);
    std::size_t remaining = vertex_count;
    std::vector<unsigned char> blocked(vertex_count, 0);
    while(remaining != 0){
        std::fill(blocked.begin(), blocked.end(), 0);
        for(const std::size_t vertex : priority){
            if(result.colors[vertex] != vertex_count || blocked[vertex]) continue;
            result.colors[vertex] = result.color_count;
            --remaining;
            for(const std::size_t neighbor : graph.adjacency[vertex]){
                if(result.colors[neighbor] == vertex_count) blocked[neighbor] = 1;
            }
        }
        ++result.color_count;
    }
    return result;
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_ADVANCED_COLORING_HPP_INCLUDED
