#pragma once

#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct KthShortestWalkEdge{
    int to;
    T cost;
};

template<class T>
std::vector<T> kth_shortest_walks(
    const std::vector<std::vector<KthShortestWalkEdge<T>>>& graph,
    int source,
    int target,
    int k
){
    const int n = static_cast<int>(graph.size());
    if(k < 0 || source < 0 || n <= source || target < 0 || n <= target)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (kth_shortest_walks).");
    }
    for(const auto& edges: graph){
        for(const auto& e: edges){
            if(e.to < 0 || n <= e.to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (kth_shortest_walks).");
            }
        }
    }
    if(k == 0) return {};

    std::vector<std::vector<T>> dist(static_cast<std::size_t>(n));
    for(auto& v: dist) v.reserve(static_cast<std::size_t>(k));

    using Pair = std::pair<T, int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> que;
    que.push({T(0), source});

    while(!que.empty() && static_cast<int>(dist[static_cast<std::size_t>(target)].size()) < k){
        auto [d, v] = que.top();
        que.pop();
        if(static_cast<int>(dist[static_cast<std::size_t>(v)].size()) == k) continue;
        dist[static_cast<std::size_t>(v)].push_back(d);
        if(v == target && static_cast<int>(dist[static_cast<std::size_t>(target)].size()) == k) break;
        for(const auto& e: graph[static_cast<std::size_t>(v)]){
            que.push({d + e.cost, e.to});
        }
    }
    return dist[static_cast<std::size_t>(target)];
}
