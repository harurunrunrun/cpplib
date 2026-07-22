#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_EULERIAN_TRAIL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_EULERIAN_TRAIL_HPP_INCLUDED

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

struct EulerianTrailResult{
    std::vector<int> vertices;
    std::vector<int> edges;
};

namespace eulerian_trail_internal{

inline std::optional<EulerianTrailResult> hierholzer(
    const std::vector<std::vector<std::pair<int, int>>>& graph,
    int edge_count,
    int start
){
    if(edge_count == 0){
        EulerianTrailResult result;
        if(start != -1) result.vertices.push_back(start);
        return result;
    }
    if(start == -1) return std::nullopt;

    std::vector<int> cursor(graph.size(), 0);
    std::vector<char> used(static_cast<std::size_t>(edge_count), 0);
    std::vector<std::pair<int, int>> stack;
    stack.reserve(static_cast<std::size_t>(edge_count) + 1);
    stack.push_back({start, -1});
    EulerianTrailResult result;
    result.vertices.reserve(static_cast<std::size_t>(edge_count) + 1);
    result.edges.reserve(static_cast<std::size_t>(edge_count));

    while(!stack.empty()){
        const int v = stack.back().first;
        int& index = cursor[static_cast<std::size_t>(v)];
        const auto& adjacency = graph[static_cast<std::size_t>(v)];
        while(index < static_cast<int>(adjacency.size()) &&
              used[static_cast<std::size_t>(adjacency[static_cast<std::size_t>(index)].second)]){
            ++index;
        }
        if(index == static_cast<int>(adjacency.size())){
            const int incoming_edge = stack.back().second;
            stack.pop_back();
            result.vertices.push_back(v);
            if(incoming_edge != -1) result.edges.push_back(incoming_edge);
            continue;
        }
        const auto [to, edge] = adjacency[static_cast<std::size_t>(index++)];
        used[static_cast<std::size_t>(edge)] = 1;
        stack.push_back({to, edge});
    }

    if(static_cast<int>(result.edges.size()) != edge_count) return std::nullopt;
    std::reverse(result.vertices.begin(), result.vertices.end());
    std::reverse(result.edges.begin(), result.edges.end());
    return result;
}

} // namespace eulerian_trail_internal

inline std::optional<EulerianTrailResult> directed_eulerian_trail(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    int start = -1
){
    if(n < 0 || start < -1 || n <= start)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (directed_eulerian_trail).");
    }
    std::vector<std::vector<std::pair<int, int>>> graph(static_cast<std::size_t>(n));
    std::vector<int> indegree(static_cast<std::size_t>(n), 0);
    std::vector<int> outdegree(static_cast<std::size_t>(n), 0);
    for(const auto& [from, to]: edges){
        if(from < 0 || n <= from || to < 0 || n <= to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (directed_eulerian_trail).");
        }
        ++outdegree[static_cast<std::size_t>(from)];
        ++indegree[static_cast<std::size_t>(to)];
    }
    for(int v = 0; v < n; ++v){
        graph[static_cast<std::size_t>(v)].reserve(
            static_cast<std::size_t>(outdegree[static_cast<std::size_t>(v)])
        );
    }
    for(int id = 0; id < static_cast<int>(edges.size()); ++id){
        const auto [from, to] = edges[static_cast<std::size_t>(id)];
        graph[static_cast<std::size_t>(from)].push_back({to, id});
    }

    int degree_start = -1;
    int degree_end = -1;
    for(int v = 0; v < n; ++v){
        const int difference = outdegree[static_cast<std::size_t>(v)] -
            indegree[static_cast<std::size_t>(v)];
        if(difference == 1){
            if(degree_start != -1) return std::nullopt;
            degree_start = v;
        }else if(difference == -1){
            if(degree_end != -1) return std::nullopt;
            degree_end = v;
        }else if(difference != 0){
            return std::nullopt;
        }
    }
    if((degree_start == -1) != (degree_end == -1)) return std::nullopt;

    int chosen_start = start;
    if(chosen_start == -1){
        chosen_start = degree_start;
        if(chosen_start == -1){
            for(int v = 0; v < n; ++v){
                if(outdegree[static_cast<std::size_t>(v)] != 0){
                    chosen_start = v;
                    break;
                }
            }
        }
        if(chosen_start == -1 && n != 0) chosen_start = 0;
    }else if(degree_start != -1 && chosen_start != degree_start){
        return std::nullopt;
    }
    return eulerian_trail_internal::hierholzer(
        graph, static_cast<int>(edges.size()), chosen_start
    );
}

inline std::optional<EulerianTrailResult> undirected_eulerian_trail(
    int n,
    const std::vector<std::pair<int, int>>& edges,
    int start = -1
){
    if(n < 0 || start < -1 || n <= start)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (undirected_eulerian_trail).");
    }
    std::vector<std::vector<std::pair<int, int>>> graph(static_cast<std::size_t>(n));
    std::vector<int> degree(static_cast<std::size_t>(n), 0);
    for(const auto& [u, v]: edges){
        if(u < 0 || n <= u || v < 0 || n <= v)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (undirected_eulerian_trail).");
        }
        ++degree[static_cast<std::size_t>(u)];
        ++degree[static_cast<std::size_t>(v)];
    }
    for(int v = 0; v < n; ++v){
        graph[static_cast<std::size_t>(v)].reserve(
            static_cast<std::size_t>(degree[static_cast<std::size_t>(v)])
        );
    }
    for(int id = 0; id < static_cast<int>(edges.size()); ++id){
        const auto [u, v] = edges[static_cast<std::size_t>(id)];
        graph[static_cast<std::size_t>(u)].push_back({v, id});
        graph[static_cast<std::size_t>(v)].push_back({u, id});
    }

    int first_odd = -1;
    int odd_count = 0;
    for(int v = 0; v < n; ++v){
        if((degree[static_cast<std::size_t>(v)] & 1) == 0) continue;
        if(first_odd == -1) first_odd = v;
        ++odd_count;
    }
    if(odd_count != 0 && odd_count != 2) return std::nullopt;

    int chosen_start = start;
    if(chosen_start == -1){
        chosen_start = first_odd;
        if(chosen_start == -1){
            for(int v = 0; v < n; ++v){
                if(degree[static_cast<std::size_t>(v)] != 0){
                    chosen_start = v;
                    break;
                }
            }
        }
        if(chosen_start == -1 && n != 0) chosen_start = 0;
    }else if(odd_count == 2 && (degree[static_cast<std::size_t>(chosen_start)] & 1) == 0){
        return std::nullopt;
    }
    return eulerian_trail_internal::hierholzer(
        graph, static_cast<int>(edges.size()), chosen_start
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_EULERIAN_TRAIL_HPP_INCLUDED
