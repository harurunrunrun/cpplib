#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_ZERO_ONE_BFS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_ZERO_ONE_BFS_HPP_INCLUDED

#include <deque>
#include <stdexcept>
#include <vector>

#include "../traversal/bfs.hpp"

struct ZeroOneBFSEdge{
    int to;
    int cost;
};

using ZeroOneBFSResult = BFSResult;

template<class EnumerateEdges>
inline ZeroOneBFSResult zero_one_bfs_implicit(
    int vertex_count,
    const std::vector<int>& sources,
    EnumerateEdges enumerate_edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (zero_one_bfs_implicit).");
    }
    for(int source: sources){
        if(source < 0 || vertex_count <= source)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (zero_one_bfs_implicit).");
        }
    }

    ZeroOneBFSResult result;
    result.dist.assign(static_cast<std::size_t>(vertex_count), -1);
    result.source.assign(static_cast<std::size_t>(vertex_count), -1);
    result.parent.assign(static_cast<std::size_t>(vertex_count), -1);
    result.reachable.assign(static_cast<std::size_t>(vertex_count), 0);
    std::vector<char> settled(static_cast<std::size_t>(vertex_count), 0);
    std::deque<int> queue;
    for(int source: sources){
        if(result.reachable[static_cast<std::size_t>(source)]) continue;
        result.dist[static_cast<std::size_t>(source)] = 0;
        result.source[static_cast<std::size_t>(source)] = source;
        result.reachable[static_cast<std::size_t>(source)] = 1;
        queue.push_back(source);
    }

    while(!queue.empty()){
        const int v = queue.front();
        queue.pop_front();
        if(settled[static_cast<std::size_t>(v)]) continue;
        settled[static_cast<std::size_t>(v)] = 1;
        enumerate_edges(v, [&](int to, int cost){
            if(to < 0 || vertex_count <= to || (cost != 0 && cost != 1))[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation (zero_one_bfs_implicit)."
                );
            }
            if(settled[static_cast<std::size_t>(to)]) return;
            const int next_dist = result.dist[static_cast<std::size_t>(v)] + cost;
            if(result.reachable[static_cast<std::size_t>(to)] &&
               result.dist[static_cast<std::size_t>(to)] <= next_dist){
                return;
            }
            result.dist[static_cast<std::size_t>(to)] = next_dist;
            result.source[static_cast<std::size_t>(to)] =
                result.source[static_cast<std::size_t>(v)];
            result.parent[static_cast<std::size_t>(to)] = v;
            result.reachable[static_cast<std::size_t>(to)] = 1;
            if(cost == 0) queue.push_front(to);
            else queue.push_back(to);
        });
    }
    return result;
}

template<class EnumerateEdges>
inline ZeroOneBFSResult zero_one_bfs_implicit(
    int vertex_count,
    int source,
    EnumerateEdges enumerate_edges
){
    return zero_one_bfs_implicit(
        vertex_count,
        std::vector<int>{source},
        enumerate_edges
    );
}

inline ZeroOneBFSResult zero_one_bfs(
    const std::vector<std::vector<ZeroOneBFSEdge>>& graph,
    const std::vector<int>& sources
){
    const int n = static_cast<int>(graph.size());
    for(const auto& edges: graph){
        for(const auto& edge: edges){
            if(edge.to < 0 || n <= edge.to || (edge.cost != 0 && edge.cost != 1))[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (zero_one_bfs).");
            }
        }
    }
    for(int source: sources){
        if(source < 0 || n <= source)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (zero_one_bfs).");
        }
    }

    ZeroOneBFSResult result;
    result.dist.assign(static_cast<std::size_t>(n), -1);
    result.source.assign(static_cast<std::size_t>(n), -1);
    result.parent.assign(static_cast<std::size_t>(n), -1);
    result.reachable.assign(static_cast<std::size_t>(n), 0);
    std::vector<char> settled(static_cast<std::size_t>(n), 0);
    std::deque<int> queue;
    for(int source: sources){
        if(result.reachable[static_cast<std::size_t>(source)]) continue;
        result.dist[static_cast<std::size_t>(source)] = 0;
        result.source[static_cast<std::size_t>(source)] = source;
        result.reachable[static_cast<std::size_t>(source)] = 1;
        queue.push_back(source);
    }

    while(!queue.empty()){
        const int v = queue.front();
        queue.pop_front();
        if(settled[static_cast<std::size_t>(v)]) continue;
        settled[static_cast<std::size_t>(v)] = 1;
        for(const auto& edge: graph[static_cast<std::size_t>(v)]){
            if(settled[static_cast<std::size_t>(edge.to)]) continue;
            const int next_dist = result.dist[static_cast<std::size_t>(v)] + edge.cost;
            if(result.reachable[static_cast<std::size_t>(edge.to)] &&
               result.dist[static_cast<std::size_t>(edge.to)] <= next_dist){
                continue;
            }
            result.dist[static_cast<std::size_t>(edge.to)] = next_dist;
            result.source[static_cast<std::size_t>(edge.to)] =
                result.source[static_cast<std::size_t>(v)];
            result.parent[static_cast<std::size_t>(edge.to)] = v;
            result.reachable[static_cast<std::size_t>(edge.to)] = 1;
            if(edge.cost == 0) queue.push_front(edge.to);
            else queue.push_back(edge.to);
        }
    }
    return result;
}

inline ZeroOneBFSResult zero_one_bfs(
    const std::vector<std::vector<ZeroOneBFSEdge>>& graph,
    int source
){
    return zero_one_bfs(graph, std::vector<int>{source});
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_ZERO_ONE_BFS_HPP_INCLUDED
