#pragma once

#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct DijkstraEdge{
    int to;
    T cost;
};

template<class T>
struct DijkstraResult{
    std::vector<T> dist;
    std::vector<int> parent;
};

template<class T>
DijkstraResult<T> dijkstra(
    const std::vector<std::vector<DijkstraEdge<T>>>& graph,
    int source,
    T inf = std::numeric_limits<T>::max() / 4
){
    const int n = static_cast<int>(graph.size());
    if(source < 0 || n <= source)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (dijkstra).");
    }
    DijkstraResult<T> result;
    result.dist.assign(static_cast<std::size_t>(n), inf);
    result.parent.assign(static_cast<std::size_t>(n), -1);
    using Pair = std::pair<T, int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> que;
    result.dist[static_cast<std::size_t>(source)] = T(0);
    que.push({T(0), source});
    while(!que.empty()){
        auto [dist, v] = que.top();
        que.pop();
        if(result.dist[static_cast<std::size_t>(v)] != dist) continue;
        for(const auto& e: graph[static_cast<std::size_t>(v)]){
            if(e.to < 0 || n <= e.to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (dijkstra).");
            }
            T nd = dist + e.cost;
            if(nd < result.dist[static_cast<std::size_t>(e.to)]){
                result.dist[static_cast<std::size_t>(e.to)] = nd;
                result.parent[static_cast<std::size_t>(e.to)] = v;
                que.push({nd, e.to});
            }
        }
    }
    return result;
}
