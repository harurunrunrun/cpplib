#pragma once

#include <functional>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

template<class T>
struct KthShortestWalkEdge{
    int to;
    T cost;
};

namespace kth_shortest_walk_internal{

template<class T>
void validate(
    const std::vector<std::vector<KthShortestWalkEdge<T>>>& graph,
    int source,
    int target
){
    const int n = static_cast<int>(graph.size());
    if(source < 0 || n <= source || target < 0 || n <= target)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (kth_shortest_walk)."
        );
    }
    for(const auto& edges: graph){
        for(const auto& edge: edges){
            if(edge.to < 0 || n <= edge.to || edge.cost < T(0))[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation (kth_shortest_walk)."
                );
            }
        }
    }
}

template<class T>
std::vector<std::optional<T>> shortest_to_target(
    const std::vector<std::vector<KthShortestWalkEdge<T>>>& graph,
    int target
){
    const int n = static_cast<int>(graph.size());
    std::vector<std::vector<KthShortestWalkEdge<T>>> reverse(
        static_cast<std::size_t>(n)
    );
    for(int from = 0; from < n; from++){
        for(const auto& edge: graph[static_cast<std::size_t>(from)]){
            reverse[static_cast<std::size_t>(edge.to)].push_back({from, edge.cost});
        }
    }

    std::vector<std::optional<T>> distance(static_cast<std::size_t>(n));
    using State = std::pair<T, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> queue;
    distance[static_cast<std::size_t>(target)] = T();
    queue.push({T(), target});
    while(!queue.empty()){
        auto [current_distance, vertex] = queue.top();
        queue.pop();
        const auto& best = distance[static_cast<std::size_t>(vertex)];
        if(*best < current_distance) continue;
        for(const auto& edge: reverse[static_cast<std::size_t>(vertex)]){
            T next_distance = current_distance + edge.cost;
            auto& next_best = distance[static_cast<std::size_t>(edge.to)];
            if(!next_best || next_distance < *next_best){
                next_best = next_distance;
                queue.push({next_distance, edge.to});
            }
        }
    }
    return distance;
}

} // namespace kth_shortest_walk_internal

namespace kth_shortest_walk_internal{

template<class T>
std::optional<T> enumerate(
    const std::vector<std::vector<KthShortestWalkEdge<T>>>& graph,
    int source,
    int target,
    std::size_t required,
    std::vector<T>* output
){
    validate(graph, source, target);
    if(required == 0) return std::nullopt;

    auto heuristic = shortest_to_target(graph, target);
    if(!heuristic[static_cast<std::size_t>(source)]) return std::nullopt;

    const int n = static_cast<int>(graph.size());
    std::vector<std::size_t> pop_count(static_cast<std::size_t>(n));
    using State = std::tuple<T, T, int>;
    std::priority_queue<State, std::vector<State>, std::greater<State>> queue;
    queue.emplace(
        *heuristic[static_cast<std::size_t>(source)],
        T(),
        source
    );

    std::size_t found = 0;
    while(!queue.empty()){
        auto [estimate, distance, vertex] = queue.top();
        (void)estimate;
        queue.pop();
        auto& count = pop_count[static_cast<std::size_t>(vertex)];
        if(count == required) continue;
        ++count;

        if(vertex == target){
            if(output != nullptr) output->push_back(distance);
            if(++found == required) return distance;
        }

        for(const auto& edge: graph[static_cast<std::size_t>(vertex)]){
            const auto& remaining = heuristic[static_cast<std::size_t>(edge.to)];
            if(!remaining) continue;
            T next_distance = distance + edge.cost;
            queue.emplace(next_distance + *remaining, next_distance, edge.to);
        }
    }
    return std::nullopt;
}

} // namespace kth_shortest_walk_internal

template<class T>
std::vector<T> kth_shortest_walks(
    const std::vector<std::vector<KthShortestWalkEdge<T>>>& graph,
    int source,
    int target,
    int k
){
    if(k < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (kth_shortest_walks).");
    }
    std::vector<T> result;
    result.reserve(static_cast<std::size_t>(k));
    (void)kth_shortest_walk_internal::enumerate(
        graph, source, target, static_cast<std::size_t>(k), &result
    );
    return result;
}

template<class T>
std::optional<T> kth_shortest_walk(
    const std::vector<std::vector<KthShortestWalkEdge<T>>>& graph,
    int source,
    int target,
    int k
){
    if(k < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (kth_shortest_walk).");
    }
    return kth_shortest_walk_internal::enumerate<T>(
        graph, source, target, static_cast<std::size_t>(k) + 1, nullptr
    );
}

template<class T>
std::optional<T> kth_shortest_walk_1indexed(
    const std::vector<std::vector<KthShortestWalkEdge<T>>>& graph,
    int source,
    int target,
    int k
){
    if(k <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (kth_shortest_walk_1indexed)."
        );
    }
    return kth_shortest_walk_internal::enumerate<T>(
        graph, source, target, static_cast<std::size_t>(k), nullptr
    );
}
