#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

template<class T>
struct MinimumSteinerTreeEdge{
    int to;
    T cost;
    int id;
};

template<class T>
struct MinimumSteinerTreeResult{
    T cost;
    std::vector<int> edge_ids;
    bool exists;
};

namespace minimum_steiner_tree_internal{

enum class DecisionKind: unsigned char{
    none,
    terminal,
    merge,
    edge,
};

struct Decision{
    DecisionKind kind = DecisionKind::none;
    int split = 0;
    int previous_vertex = -1;
    int edge_index = -1;
};

} // namespace minimum_steiner_tree_internal

template<class T>
MinimumSteinerTreeResult<T> minimum_steiner_tree(
    const std::vector<std::vector<MinimumSteinerTreeEdge<T>>>& graph,
    const std::vector<int>& terminals,
    T infinity = std::numeric_limits<T>::max() / 4
){
    using namespace minimum_steiner_tree_internal;
    const int vertex_count = static_cast<int>(graph.size());
    const int terminal_count = static_cast<int>(terminals.size());
    for(int terminal: terminals){
        if(terminal < 0 || vertex_count <= terminal)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (minimum_steiner_tree)."
            );
        }
    }
    for(const auto& edges: graph){
        for(const auto& edge: edges){
            if(edge.to < 0 || vertex_count <= edge.to ||
               edge.cost < T(0) || edge.id < 0)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation (minimum_steiner_tree)."
                );
            }
        }
    }
    if(terminal_count == 0) return {T(0), {}, true};
    if(terminal_count >= std::numeric_limits<int>::digits)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: too many terminals (minimum_steiner_tree)."
        );
    }

    const int subset_count = 1 << terminal_count;
    const std::size_t state_count = static_cast<std::size_t>(subset_count) *
        static_cast<std::size_t>(vertex_count);
    std::vector<T> distance(state_count, infinity);
    std::vector<unsigned char> reachable(state_count, 0);
    std::vector<Decision> decision(state_count);
    const auto state = [vertex_count](int subset, int vertex){
        return static_cast<std::size_t>(subset) *
            static_cast<std::size_t>(vertex_count) +
            static_cast<std::size_t>(vertex);
    };

    for(int index = 0; index < terminal_count; ++index){
        const std::size_t position = state(
            1 << index, terminals[static_cast<std::size_t>(index)]
        );
        distance[position] = T(0);
        reachable[position] = 1;
        decision[position].kind = DecisionKind::terminal;
    }

    using QueueEntry = std::pair<T, int>;
    for(int subset = 1; subset < subset_count; ++subset){
        for(int part = (subset - 1) & subset; part != 0;
            part = (part - 1) & subset){
            const int other = subset ^ part;
            if(other == 0 || other < part) continue;
            for(int vertex = 0; vertex < vertex_count; ++vertex){
                const std::size_t left = state(part, vertex);
                const std::size_t right = state(other, vertex);
                if(!reachable[left] || !reachable[right]) continue;
                const T candidate = distance[left] + distance[right];
                const std::size_t current = state(subset, vertex);
                if(!reachable[current] || candidate < distance[current]){
                    distance[current] = candidate;
                    reachable[current] = 1;
                    decision[current] = {
                        DecisionKind::merge, part, -1, -1
                    };
                }
            }
        }

        std::priority_queue<
            QueueEntry,
            std::vector<QueueEntry>,
            std::greater<QueueEntry>
        > queue;
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            const std::size_t current = state(subset, vertex);
            if(reachable[current]) queue.push({distance[current], vertex});
        }
        while(!queue.empty()){
            const auto [current_distance, vertex] = queue.top();
            queue.pop();
            const std::size_t current = state(subset, vertex);
            if(distance[current] != current_distance) continue;
            const auto& edges = graph[static_cast<std::size_t>(vertex)];
            for(int edge_index = 0;
                edge_index < static_cast<int>(edges.size());
                ++edge_index){
                const auto& edge = edges[static_cast<std::size_t>(edge_index)];
                const T candidate = current_distance + edge.cost;
                const std::size_t next = state(subset, edge.to);
                if(!reachable[next] || candidate < distance[next]){
                    distance[next] = candidate;
                    reachable[next] = 1;
                    decision[next] = {
                        DecisionKind::edge, 0, vertex, edge_index
                    };
                    queue.push({candidate, edge.to});
                }
            }
        }
    }

    const int full = subset_count - 1;
    int best_vertex = -1;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        const std::size_t current = state(full, vertex);
        if(reachable[current] &&
           (best_vertex == -1 ||
            distance[current] < distance[state(full, best_vertex)])){
            best_vertex = vertex;
        }
    }
    if(best_vertex == -1) return {infinity, {}, false};

    std::vector<std::pair<int, int>> stack{{full, best_vertex}};
    std::unordered_set<int> used_edges;
    std::vector<int> edge_ids;
    while(!stack.empty()){
        const auto [subset, vertex] = stack.back();
        stack.pop_back();
        const Decision& current = decision[state(subset, vertex)];
        if(current.kind == DecisionKind::terminal) continue;
        if(current.kind == DecisionKind::merge){
            stack.push_back({current.split, vertex});
            stack.push_back({subset ^ current.split, vertex});
            continue;
        }
        if(current.kind == DecisionKind::edge){
            const auto& edge = graph[static_cast<std::size_t>(
                current.previous_vertex
            )][static_cast<std::size_t>(current.edge_index)];
            if(used_edges.insert(edge.id).second) edge_ids.push_back(edge.id);
            stack.push_back({subset, current.previous_vertex});
            continue;
        }
        throw std::runtime_error(
            "library assertion fault: reconstruction failed (minimum_steiner_tree)."
        );
    }
    std::sort(edge_ids.begin(), edge_ids.end());
    return {distance[state(full, best_vertex)], std::move(edge_ids), true};
}
