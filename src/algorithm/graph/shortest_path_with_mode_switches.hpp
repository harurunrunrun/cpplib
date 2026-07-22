#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_WITH_MODE_SWITCHES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_WITH_MODE_SWITCHES_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "zero_one_bfs.hpp"

struct ModeSwitchEdge{
    int from;
    int to;
    int mode;
};

inline int shortest_path_with_mode_switches(
    int vertex_count,
    const std::vector<ModeSwitchEdge>& edges,
    const std::vector<int>& switch_vertices,
    int source = 0,
    int target = -1,
    int initial_mode = 1
){
    if(vertex_count <= 0 ||
       vertex_count > std::numeric_limits<int>::max() / 2)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(shortest_path_with_mode_switches)."
        );
    }
    if(target == -1) target = vertex_count - 1;
    if(source < 0 || vertex_count <= source ||
       target < 0 || vertex_count <= target ||
       (initial_mode != 0 && initial_mode != 1))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(shortest_path_with_mode_switches)."
        );
    }

    std::vector<std::vector<ZeroOneBFSEdge>> graph(
        static_cast<std::size_t>(2 * vertex_count)
    );
    for(const auto& edge: edges){
        if(edge.from < 0 || vertex_count <= edge.from ||
           edge.to < 0 || vertex_count <= edge.to ||
           (edge.mode != 0 && edge.mode != 1))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(shortest_path_with_mode_switches)."
            );
        }
        const int from = edge.from + edge.mode * vertex_count;
        const int to = edge.to + edge.mode * vertex_count;
        graph[static_cast<std::size_t>(from)].push_back({to, 1});
        graph[static_cast<std::size_t>(to)].push_back({from, 1});
    }
    for(int vertex: switch_vertices){
        if(vertex < 0 || vertex_count <= vertex)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(shortest_path_with_mode_switches)."
            );
        }
        graph[static_cast<std::size_t>(vertex)].push_back({vertex + vertex_count, 0});
        graph[static_cast<std::size_t>(vertex + vertex_count)].push_back({vertex, 0});
    }

    const auto result = zero_one_bfs(
        graph,
        source + initial_mode * vertex_count
    );
    const int distance_zero = result.dist[static_cast<std::size_t>(target)];
    const int distance_one =
        result.dist[static_cast<std::size_t>(target + vertex_count)];
    if(distance_zero == -1) return distance_one;
    if(distance_one == -1) return distance_zero;
    return std::min(distance_zero, distance_one);
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_WITH_MODE_SWITCHES_HPP_INCLUDED
