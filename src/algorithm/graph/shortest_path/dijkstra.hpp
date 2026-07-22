#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_DIJKSTRA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_DIJKSTRA_HPP_INCLUDED

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
    std::vector<char> reachable;
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
    for(const auto& edges: graph){
        for(const auto& e: edges){
            if(e.to < 0 || n <= e.to || e.cost < T(0))[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (dijkstra).");
            }
        }
    }
    DijkstraResult<T> result;
    result.dist.assign(static_cast<std::size_t>(n), inf);
    result.parent.assign(static_cast<std::size_t>(n), -1);
    result.reachable.assign(static_cast<std::size_t>(n), 0);
    using Pair = std::pair<T, int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> que;
    result.dist[static_cast<std::size_t>(source)] = T(0);
    result.reachable[static_cast<std::size_t>(source)] = 1;
    que.push({T(0), source});
    while(!que.empty()){
        auto [dist, v] = que.top();
        que.pop();
        if(result.dist[static_cast<std::size_t>(v)] != dist) continue;
        for(const auto& e: graph[static_cast<std::size_t>(v)]){
            T nd = dist + e.cost;
            if(!result.reachable[static_cast<std::size_t>(e.to)] ||
               nd < result.dist[static_cast<std::size_t>(e.to)]){
                result.dist[static_cast<std::size_t>(e.to)] = nd;
                result.reachable[static_cast<std::size_t>(e.to)] = 1;
                result.parent[static_cast<std::size_t>(e.to)] = v;
                que.push({nd, e.to});
            }
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_DIJKSTRA_HPP_INCLUDED
