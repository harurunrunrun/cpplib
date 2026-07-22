#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_MULTI_SOURCE_SHORTEST_PATH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_MULTI_SOURCE_SHORTEST_PATH_HPP_INCLUDED

#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct MultiSourceShortestPathEdge{
    int to;
    T cost;
};

template<class T>
struct MultiSourceShortestPathResult{
    std::vector<T> dist;
    std::vector<int> source;
    std::vector<int> parent;
    std::vector<char> reachable;
};

template<class T>
MultiSourceShortestPathResult<T> multi_source_shortest_path(
    const std::vector<std::vector<MultiSourceShortestPathEdge<T>>>& graph,
    const std::vector<int>& sources,
    T inf = std::numeric_limits<T>::max() / 4
){
    const int n = static_cast<int>(graph.size());
    for(int s: sources){
        if(s < 0 || n <= s)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (multi_source_shortest_path).");
        }
    }
    for(const auto& edges: graph){
        for(const auto& e: edges){
            if(e.to < 0 || n <= e.to || e.cost < T(0))[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (multi_source_shortest_path).");
            }
        }
    }

    MultiSourceShortestPathResult<T> result;
    result.dist.assign(static_cast<std::size_t>(n), inf);
    result.source.assign(static_cast<std::size_t>(n), -1);
    result.parent.assign(static_cast<std::size_t>(n), -1);
    result.reachable.assign(static_cast<std::size_t>(n), 0);

    using Pair = std::pair<T, int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> que;
    for(int s: sources){
        if(result.reachable[static_cast<std::size_t>(s)]) continue;
        result.dist[static_cast<std::size_t>(s)] = T(0);
        result.source[static_cast<std::size_t>(s)] = s;
        result.reachable[static_cast<std::size_t>(s)] = 1;
        que.push({T(0), s});
    }

    while(!que.empty()){
        auto [d, v] = que.top();
        que.pop();
        if(result.dist[static_cast<std::size_t>(v)] != d) continue;
        for(const auto& e: graph[static_cast<std::size_t>(v)]){
            T nd = d + e.cost;
            if(!result.reachable[static_cast<std::size_t>(e.to)] ||
               nd < result.dist[static_cast<std::size_t>(e.to)]){
                result.dist[static_cast<std::size_t>(e.to)] = nd;
                result.source[static_cast<std::size_t>(e.to)] = result.source[static_cast<std::size_t>(v)];
                result.parent[static_cast<std::size_t>(e.to)] = v;
                result.reachable[static_cast<std::size_t>(e.to)] = 1;
                que.push({nd, e.to});
            }
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_MULTI_SOURCE_SHORTEST_PATH_HPP_INCLUDED
