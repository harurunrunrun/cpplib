#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_SPFA_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_SPFA_HPP_INCLUDED

#include <cstddef>
#include <deque>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct SpfaEdge{
    int to;
    T cost;
};

template<class T>
struct SpfaResult{
    std::vector<T> distance;
    std::vector<char> reachable;
    std::vector<char> negative_cycle_affected;
    std::vector<int> parent;
    bool has_reachable_negative_cycle = false;
};

template<class T>
SpfaResult<T> spfa(
    const std::vector<std::vector<SpfaEdge<T>>>& graph,
    const std::vector<int>& sources,
    T inf = std::numeric_limits<T>::max() / 4
){
    if(graph.size()
        > static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: size violation (spfa)."
        );
    }
    const int vertex_count = static_cast<int>(graph.size());
    for(const auto& edges: graph){
        for(const auto& edge: edges){
            if(edge.to < 0 || vertex_count <= edge.to)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation (spfa)."
                );
            }
        }
    }
    for(int source: sources){
        if(source < 0 || vertex_count <= source)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (spfa)."
            );
        }
    }

    SpfaResult<T> result;
    result.distance.assign(static_cast<std::size_t>(vertex_count), inf);
    result.reachable.assign(static_cast<std::size_t>(vertex_count), 0);
    result.negative_cycle_affected.assign(
        static_cast<std::size_t>(vertex_count), 0
    );
    result.parent.assign(static_cast<std::size_t>(vertex_count), -1);

    std::vector<int> path_edge_count(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::vector<char> in_queue(static_cast<std::size_t>(vertex_count), 0);
    std::vector<char> negative_seed(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::deque<int> queue;
    for(int source: sources){
        const std::size_t index = static_cast<std::size_t>(source);
        if(result.reachable[index]) continue;
        result.reachable[index] = 1;
        result.distance[index] = T(0);
        in_queue[index] = 1;
        queue.push_back(source);
    }

    while(!queue.empty()){
        const int vertex = queue.front();
        queue.pop_front();
        const std::size_t vertex_index = static_cast<std::size_t>(vertex);
        in_queue[vertex_index] = 0;
        if(negative_seed[vertex_index]) continue;

        for(const auto& edge: graph[vertex_index]){
            const std::size_t to = static_cast<std::size_t>(edge.to);
            if(negative_seed[to]) continue;
            const T candidate = result.distance[vertex_index] + edge.cost;
            if(result.reachable[to] && !(candidate < result.distance[to])){
                continue;
            }
            result.reachable[to] = 1;
            result.distance[to] = candidate;
            result.parent[to] = vertex;
            path_edge_count[to] = path_edge_count[vertex_index] + 1;
            if(path_edge_count[to] >= vertex_count){
                negative_seed[to] = 1;
                result.has_reachable_negative_cycle = true;
                continue;
            }
            if(!in_queue[to]){
                in_queue[to] = 1;
                queue.push_back(edge.to);
            }
        }
    }

    std::deque<int> affected_queue;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(!negative_seed[static_cast<std::size_t>(vertex)]) continue;
        result.negative_cycle_affected[static_cast<std::size_t>(vertex)] = 1;
        affected_queue.push_back(vertex);
    }
    while(!affected_queue.empty()){
        const int vertex = affected_queue.front();
        affected_queue.pop_front();
        for(const auto& edge: graph[static_cast<std::size_t>(vertex)]){
            const std::size_t to = static_cast<std::size_t>(edge.to);
            result.reachable[to] = 1;
            if(result.negative_cycle_affected[to]) continue;
            result.negative_cycle_affected[to] = 1;
            affected_queue.push_back(edge.to);
        }
    }
    return result;
}

template<class T>
SpfaResult<T> spfa(
    const std::vector<std::vector<SpfaEdge<T>>>& graph,
    int source,
    T inf = std::numeric_limits<T>::max() / 4
){
    return spfa(graph, std::vector<int>{source}, inf);
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SHORTEST_PATH_SPFA_HPP_INCLUDED
