#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_CYCLE_DETECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_CYCLE_DETECTION_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

struct CycleDetectionResult{
    std::vector<int> vertices;
    std::vector<int> edges;
};

inline CycleDetectionResult directed_cycle_with_edges(
    int n,
    const std::vector<std::pair<int, int>>& edges
){
    if(n < 0 || edges.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (directed_cycle_with_edges).");
    }
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(int id = 0; id < static_cast<int>(edges.size()); ++id){
        const auto [from, to] = edges[static_cast<std::size_t>(id)];
        if(from < 0 || n <= from || to < 0 || n <= to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (directed_cycle_with_edges).");
        }
        graph[static_cast<std::size_t>(from)].push_back(id);
    }

    std::vector<char> color(static_cast<std::size_t>(n), 0);
    std::vector<int> parent(static_cast<std::size_t>(n), -1);
    std::vector<int> parent_edge(static_cast<std::size_t>(n), -1);
    std::vector<std::size_t> next_edge(static_cast<std::size_t>(n), 0);
    std::vector<int> stack;
    CycleDetectionResult result;
    for(int root = 0; root < n && result.edges.empty(); ++root){
        if(color[static_cast<std::size_t>(root)] != 0) continue;
        color[static_cast<std::size_t>(root)] = 1;
        stack.push_back(root);
        while(!stack.empty() && result.edges.empty()){
            const int vertex = stack.back();
            auto& index = next_edge[static_cast<std::size_t>(vertex)];
            if(index == graph[static_cast<std::size_t>(vertex)].size()){
                color[static_cast<std::size_t>(vertex)] = 2;
                stack.pop_back();
                continue;
            }
            const int id = graph[static_cast<std::size_t>(vertex)][index++];
            const int to = edges[static_cast<std::size_t>(id)].second;
            if(color[static_cast<std::size_t>(to)] == 0){
                parent[static_cast<std::size_t>(to)] = vertex;
                parent_edge[static_cast<std::size_t>(to)] = id;
                color[static_cast<std::size_t>(to)] = 1;
                stack.push_back(to);
            }else if(color[static_cast<std::size_t>(to)] == 1){
                for(int x = vertex; x != to; x = parent[static_cast<std::size_t>(x)]){
                    result.vertices.push_back(x);
                    result.edges.push_back(parent_edge[static_cast<std::size_t>(x)]);
                }
                result.vertices.push_back(to);
                std::reverse(result.vertices.begin(), result.vertices.end());
                std::reverse(result.edges.begin(), result.edges.end());
                result.edges.push_back(id);
            }
        }
    }
    return result;
}

inline std::vector<int> directed_cycle(const std::vector<std::vector<int>>& graph){
    if(graph.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::runtime_error("library assertion fault: too many vertices (directed_cycle).");
    }
    const int n = static_cast<int>(graph.size());
    std::vector<std::pair<int, int>> edges;
    for(int from = 0; from < n; ++from){
        const auto& adjacent = graph[static_cast<std::size_t>(from)];
        if(adjacent.size() >
           static_cast<std::size_t>(std::numeric_limits<int>::max()) - edges.size())[[unlikely]]{
            throw std::runtime_error("library assertion fault: too many edges (directed_cycle).");
        }
        for(int to: adjacent){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (directed_cycle).");
            }
            edges.push_back({from, to});
        }
    }
    return directed_cycle_with_edges(n, edges).vertices;
}

inline bool has_directed_cycle(const std::vector<std::vector<int>>& graph){
    return !directed_cycle(graph).empty();
}

inline CycleDetectionResult undirected_cycle_with_edges(
    int n,
    const std::vector<std::pair<int, int>>& edges
){
    if(n < 0 || edges.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (undirected_cycle_with_edges).");
    }
    std::vector<std::vector<std::pair<int, int>>> graph(static_cast<std::size_t>(n));
    for(int id = 0; id < static_cast<int>(edges.size()); ++id){
        const auto [from, to] = edges[static_cast<std::size_t>(id)];
        if(from < 0 || n <= from || to < 0 || n <= to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (undirected_cycle_with_edges).");
        }
        graph[static_cast<std::size_t>(from)].push_back({to, id});
        graph[static_cast<std::size_t>(to)].push_back({from, id});
    }

    std::vector<char> color(static_cast<std::size_t>(n), 0);
    std::vector<int> parent(static_cast<std::size_t>(n), -1);
    std::vector<int> parent_edge(static_cast<std::size_t>(n), -1);
    std::vector<std::size_t> next_edge(static_cast<std::size_t>(n), 0);
    std::vector<int> stack;
    CycleDetectionResult result;
    for(int root = 0; root < n && result.edges.empty(); ++root){
        if(color[static_cast<std::size_t>(root)] != 0) continue;
        color[static_cast<std::size_t>(root)] = 1;
        stack.push_back(root);
        while(!stack.empty() && result.edges.empty()){
            const int vertex = stack.back();
            auto& index = next_edge[static_cast<std::size_t>(vertex)];
            if(index == graph[static_cast<std::size_t>(vertex)].size()){
                color[static_cast<std::size_t>(vertex)] = 2;
                stack.pop_back();
                continue;
            }
            const auto [to, id] = graph[static_cast<std::size_t>(vertex)][index++];
            if(id == parent_edge[static_cast<std::size_t>(vertex)]) continue;
            if(color[static_cast<std::size_t>(to)] == 0){
                parent[static_cast<std::size_t>(to)] = vertex;
                parent_edge[static_cast<std::size_t>(to)] = id;
                color[static_cast<std::size_t>(to)] = 1;
                stack.push_back(to);
            }else if(color[static_cast<std::size_t>(to)] == 1){
                for(int x = vertex; x != to; x = parent[static_cast<std::size_t>(x)]){
                    result.vertices.push_back(x);
                    result.edges.push_back(parent_edge[static_cast<std::size_t>(x)]);
                }
                result.vertices.push_back(to);
                std::reverse(result.vertices.begin(), result.vertices.end());
                std::reverse(result.edges.begin(), result.edges.end());
                result.edges.push_back(id);
            }
        }
    }
    return result;
}

inline std::vector<int> undirected_cycle(
    int n,
    const std::vector<std::pair<int, int>>& edges
){
    return undirected_cycle_with_edges(n, edges).vertices;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_CYCLE_DETECTION_HPP_INCLUDED
