#pragma once

#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <vector>

template<class T>
struct PrimEdge{
    int to;
    T cost;
};

template<class T>
struct PrimTreeEdge{
    int from;
    int to;
    T cost;
};

template<class T>
struct PrimMstResult{
    T cost;
    std::vector<PrimTreeEdge<T>> edges;
    bool connected;
};

template<class T>
PrimMstResult<T> prim_mst(const std::vector<std::vector<PrimEdge<T>>>& graph){
    const int n = static_cast<int>(graph.size());
    for(const auto& edges: graph){
        for(const auto& e: edges){
            if(e.to < 0 || n <= e.to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (prim_mst).");
            }
        }
    }

    PrimMstResult<T> result;
    result.cost = T(0);
    result.connected = true;
    std::vector<char> used(static_cast<std::size_t>(n), 0);
    using QueueValue = std::tuple<T, int, int>;
    std::priority_queue<QueueValue, std::vector<QueueValue>, std::greater<QueueValue>> que;

    int components = 0;
    for(int start = 0; start < n; start++){
        if(used[static_cast<std::size_t>(start)]) continue;
        components++;
        used[static_cast<std::size_t>(start)] = 1;
        for(const auto& e: graph[static_cast<std::size_t>(start)]){
            que.push({e.cost, start, e.to});
        }
        while(!que.empty()){
            auto [cost, from, to] = que.top();
            que.pop();
            if(used[static_cast<std::size_t>(to)]) continue;
            used[static_cast<std::size_t>(to)] = 1;
            result.cost += cost;
            result.edges.push_back({from, to, cost});
            for(const auto& e: graph[static_cast<std::size_t>(to)]){
                if(!used[static_cast<std::size_t>(e.to)]){
                    que.push({e.cost, to, e.to});
                }
            }
        }
    }
    result.connected = components <= 1;
    return result;
}
