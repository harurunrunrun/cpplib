#pragma once

#include <limits>
#include <stdexcept>
#include <vector>

template<class T>
struct BellmanFordEdge{
    int from;
    int to;
    T cost;
};

template<class T>
struct BellmanFordResult{
    std::vector<T> dist;
    std::vector<int> parent;
    std::vector<char> negative;
    bool has_negative_cycle = false;
};

template<class T>
BellmanFordResult<T> bellman_ford(
    int n,
    const std::vector<BellmanFordEdge<T>>& edges,
    int source,
    T inf = std::numeric_limits<T>::max() / 4
){
    if(n < 0 || source < 0 || n <= source)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (bellman_ford).");
    }
    for(const auto& e: edges){
        if(e.from < 0 || n <= e.from || e.to < 0 || n <= e.to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (bellman_ford).");
        }
    }

    BellmanFordResult<T> result;
    result.dist.assign(static_cast<std::size_t>(n), inf);
    result.parent.assign(static_cast<std::size_t>(n), -1);
    result.negative.assign(static_cast<std::size_t>(n), 0);
    result.dist[static_cast<std::size_t>(source)] = T(0);

    for(int iter = 0; iter < n - 1; iter++){
        bool updated = false;
        for(const auto& e: edges){
            if(result.dist[static_cast<std::size_t>(e.from)] == inf) continue;
            T nd = result.dist[static_cast<std::size_t>(e.from)] + e.cost;
            if(nd < result.dist[static_cast<std::size_t>(e.to)]){
                result.dist[static_cast<std::size_t>(e.to)] = nd;
                result.parent[static_cast<std::size_t>(e.to)] = e.from;
                updated = true;
            }
        }
        if(!updated) break;
    }

    for(int iter = 0; iter < n; iter++){
        for(const auto& e: edges){
            if(result.dist[static_cast<std::size_t>(e.from)] == inf) continue;
            T nd = result.dist[static_cast<std::size_t>(e.from)] + e.cost;
            if(nd < result.dist[static_cast<std::size_t>(e.to)] ||
               result.negative[static_cast<std::size_t>(e.from)]){
                result.negative[static_cast<std::size_t>(e.to)] = 1;
            }
        }
    }
    for(char x: result.negative){
        if(x){
            result.has_negative_cycle = true;
            break;
        }
    }
    return result;
}
