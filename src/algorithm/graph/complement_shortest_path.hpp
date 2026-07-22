#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_COMPLEMENT_SHORTEST_PATH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_COMPLEMENT_SHORTEST_PATH_HPP_INCLUDED

#include <queue>
#include <stdexcept>
#include <vector>

struct ComplementShortestPathResult{
    std::vector<int> dist;
    std::vector<int> parent;
};

ComplementShortestPathResult complement_shortest_path(
    const std::vector<std::vector<int>>& graph,
    int source
){
    const int n = static_cast<int>(graph.size());
    if(source < 0 || n <= source)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (complement_shortest_path).");
    }
    for(const auto& edges: graph){
        for(int to: edges){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (complement_shortest_path).");
            }
        }
    }

    ComplementShortestPathResult result;
    result.dist.assign(static_cast<std::size_t>(n), -1);
    result.parent.assign(static_cast<std::size_t>(n), -1);

    std::vector<int> remaining;
    remaining.reserve(static_cast<std::size_t>(n));
    for(int v = 0; v < n; v++){
        if(v != source) remaining.push_back(v);
    }

    std::vector<int> marked(static_cast<std::size_t>(n), 0);
    int stamp = 0;
    std::queue<int> que;
    result.dist[static_cast<std::size_t>(source)] = 0;
    que.push(source);

    while(!que.empty()){
        int v = que.front();
        que.pop();
        stamp++;
        for(int to: graph[static_cast<std::size_t>(v)]){
            marked[static_cast<std::size_t>(to)] = stamp;
        }

        std::vector<int> next_remaining;
        next_remaining.reserve(remaining.size());
        for(int u: remaining){
            if(marked[static_cast<std::size_t>(u)] == stamp){
                next_remaining.push_back(u);
                continue;
            }
            result.dist[static_cast<std::size_t>(u)] = result.dist[static_cast<std::size_t>(v)] + 1;
            result.parent[static_cast<std::size_t>(u)] = v;
            que.push(u);
        }
        remaining.swap(next_remaining);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_COMPLEMENT_SHORTEST_PATH_HPP_INCLUDED
