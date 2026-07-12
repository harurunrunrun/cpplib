#pragma once

#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct ShortestPathEdgeUsageEdge{
    int from;
    int to;
    T cost;
};

template<class T>
struct ShortestPathEdgeUsageResult{
    T shortest;
    std::vector<char> used;
    std::vector<char> unused;
};

namespace shortest_path_edge_usage_internal{

template<class T>
std::vector<T> dijkstra(
    const std::vector<std::vector<std::pair<int, T>>>& graph,
    int source,
    T inf
){
    const int n = static_cast<int>(graph.size());
    std::vector<T> dist(static_cast<std::size_t>(n), inf);
    using Pair = std::pair<T, int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> que;
    dist[static_cast<std::size_t>(source)] = T(0);
    que.push({T(0), source});
    while(!que.empty()){
        auto [d, v] = que.top();
        que.pop();
        if(dist[static_cast<std::size_t>(v)] != d) continue;
        for(auto [to, cost]: graph[static_cast<std::size_t>(v)]){
            T nd = d + cost;
            if(nd < dist[static_cast<std::size_t>(to)]){
                dist[static_cast<std::size_t>(to)] = nd;
                que.push({nd, to});
            }
        }
    }
    return dist;
}

} // namespace shortest_path_edge_usage_internal

template<class T>
ShortestPathEdgeUsageResult<T> shortest_path_edge_usage(
    int n,
    const std::vector<ShortestPathEdgeUsageEdge<T>>& edges,
    int source,
    int target,
    T inf = std::numeric_limits<T>::max() / 4
){
    if(n < 0 || source < 0 || n <= source || target < 0 || n <= target)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (shortest_path_edge_usage).");
    }
    std::vector<std::vector<std::pair<int, T>>> graph(static_cast<std::size_t>(n));
    std::vector<std::vector<std::pair<int, T>>> reverse_graph(static_cast<std::size_t>(n));
    for(const auto& e: edges){
        if(e.from < 0 || n <= e.from || e.to < 0 || n <= e.to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (shortest_path_edge_usage).");
        }
        graph[static_cast<std::size_t>(e.from)].push_back({e.to, e.cost});
        reverse_graph[static_cast<std::size_t>(e.to)].push_back({e.from, e.cost});
    }

    auto from_source = shortest_path_edge_usage_internal::dijkstra(graph, source, inf);
    auto to_target = shortest_path_edge_usage_internal::dijkstra(reverse_graph, target, inf);

    ShortestPathEdgeUsageResult<T> result;
    result.shortest = from_source[static_cast<std::size_t>(target)];
    result.used.assign(edges.size(), 0);
    result.unused.assign(edges.size(), 1);
    if(result.shortest == inf) return result;

    for(std::size_t i = 0; i < edges.size(); i++){
        const auto& e = edges[i];
        if(from_source[static_cast<std::size_t>(e.from)] == inf ||
           to_target[static_cast<std::size_t>(e.to)] == inf){
            continue;
        }
        if(from_source[static_cast<std::size_t>(e.from)] + e.cost +
               to_target[static_cast<std::size_t>(e.to)] ==
           result.shortest){
            result.used[i] = 1;
            result.unused[i] = 0;
        }
    }
    return result;
}
