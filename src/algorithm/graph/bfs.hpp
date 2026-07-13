#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

struct BFSResult{
    std::vector<int> dist;
    std::vector<int> source;
    std::vector<int> parent;
    std::vector<char> reachable;

    std::vector<int> path(int target) const{
        const int n = static_cast<int>(dist.size());
        if(target < 0 || n <= target)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (BFSResult::path).");
        }
        if(!reachable[static_cast<std::size_t>(target)]) return {};
        std::vector<int> result;
        for(int v = target; v != -1; v = parent[static_cast<std::size_t>(v)]){
            result.push_back(v);
        }
        std::reverse(result.begin(), result.end());
        return result;
    }
};

inline BFSResult bfs(
    const std::vector<std::vector<int>>& graph,
    const std::vector<int>& sources
){
    const int n = static_cast<int>(graph.size());
    for(const auto& edges: graph){
        for(int to: edges){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (bfs).");
            }
        }
    }
    for(int source: sources){
        if(source < 0 || n <= source)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (bfs).");
        }
    }

    BFSResult result;
    result.dist.assign(static_cast<std::size_t>(n), -1);
    result.source.assign(static_cast<std::size_t>(n), -1);
    result.parent.assign(static_cast<std::size_t>(n), -1);
    result.reachable.assign(static_cast<std::size_t>(n), 0);
    std::vector<int> queue(static_cast<std::size_t>(n));
    int head = 0;
    int tail = 0;
    for(int source: sources){
        if(result.reachable[static_cast<std::size_t>(source)]) continue;
        result.dist[static_cast<std::size_t>(source)] = 0;
        result.source[static_cast<std::size_t>(source)] = source;
        result.reachable[static_cast<std::size_t>(source)] = 1;
        queue[static_cast<std::size_t>(tail++)] = source;
    }
    while(head < tail){
        const int v = queue[static_cast<std::size_t>(head++)];
        for(int to: graph[static_cast<std::size_t>(v)]){
            if(result.reachable[static_cast<std::size_t>(to)]) continue;
            result.dist[static_cast<std::size_t>(to)] =
                result.dist[static_cast<std::size_t>(v)] + 1;
            result.source[static_cast<std::size_t>(to)] =
                result.source[static_cast<std::size_t>(v)];
            result.parent[static_cast<std::size_t>(to)] = v;
            result.reachable[static_cast<std::size_t>(to)] = 1;
            queue[static_cast<std::size_t>(tail++)] = to;
        }
    }
    return result;
}

inline BFSResult bfs(const std::vector<std::vector<int>>& graph, int source){
    return bfs(graph, std::vector<int>{source});
}
