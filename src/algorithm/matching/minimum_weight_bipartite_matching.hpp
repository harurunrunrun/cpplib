#pragma once

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct MinimumWeightBipartiteMatchingEdge{
    int left;
    int right;
    T cost;
};

template<class T>
struct MinimumWeightBipartiteMatchingResult{
    int size;
    T cost;
    std::vector<int> left_match;
    std::vector<int> right_match;
};

template<class T>
MinimumWeightBipartiteMatchingResult<T> minimum_weight_bipartite_matching(
    int left_size,
    int right_size,
    const std::vector<MinimumWeightBipartiteMatchingEdge<T>>& input_edges,
    T inf = std::numeric_limits<T>::max() / 4
){
    if(left_size < 0 || right_size < 0
        || left_size > std::numeric_limits<int>::max() - right_size - 2
        || input_edges.size() > static_cast<std::size_t>(
            std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(minimum_weight_bipartite_matching)."
        );
    }
    const int source = left_size + right_size;
    const int sink = source + 1;
    const int vertex_count = sink + 1;

    struct Edge{
        int to;
        int reverse;
        int cap;
        T cost;
        int input_id;
    };
    std::vector<std::vector<Edge>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    const auto add_edge = [&](int from, int to, int cap, T cost, int input_id){
        const int reverse_from =
            static_cast<int>(graph[static_cast<std::size_t>(to)].size());
        const int reverse_to =
            static_cast<int>(graph[static_cast<std::size_t>(from)].size());
        graph[static_cast<std::size_t>(from)].push_back({
            to, reverse_from, cap, cost, input_id,
        });
        graph[static_cast<std::size_t>(to)].push_back({
            from, reverse_to, 0, -cost, -1,
        });
    };

    for(int left = 0; left < left_size; ++left){
        add_edge(source, left, 1, T(0), -1);
    }
    for(int right = 0; right < right_size; ++right){
        add_edge(left_size + right, sink, 1, T(0), -1);
    }
    for(int id = 0; id < static_cast<int>(input_edges.size()); ++id){
        const auto& edge = input_edges[static_cast<std::size_t>(id)];
        if(edge.left < 0 || left_size <= edge.left
            || edge.right < 0 || right_size <= edge.right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(minimum_weight_bipartite_matching)."
            );
        }
        add_edge(edge.left, left_size + edge.right, 1, edge.cost, id);
    }

    std::vector<T> potential(static_cast<std::size_t>(vertex_count), T(0));
    std::vector<char> reached(static_cast<std::size_t>(vertex_count), 0);
    reached[static_cast<std::size_t>(source)] = 1;
    for(int left = 0; left < left_size; ++left){
        reached[static_cast<std::size_t>(left)] = 1;
    }
    for(const auto& edge: input_edges){
        const int right_vertex = left_size + edge.right;
        if(!reached[static_cast<std::size_t>(right_vertex)]
            || edge.cost < potential[static_cast<std::size_t>(right_vertex)]){
            reached[static_cast<std::size_t>(right_vertex)] = 1;
            potential[static_cast<std::size_t>(right_vertex)] = edge.cost;
        }
    }
    for(int right = 0; right < right_size; ++right){
        const int right_vertex = left_size + right;
        if(!reached[static_cast<std::size_t>(right_vertex)]) continue;
        if(!reached[static_cast<std::size_t>(sink)]
            || potential[static_cast<std::size_t>(right_vertex)]
                < potential[static_cast<std::size_t>(sink)]){
            reached[static_cast<std::size_t>(sink)] = 1;
            potential[static_cast<std::size_t>(sink)] =
                potential[static_cast<std::size_t>(right_vertex)];
        }
    }
    std::vector<T> distance(static_cast<std::size_t>(vertex_count), inf);

    MinimumWeightBipartiteMatchingResult<T> result;
    result.size = 0;
    result.cost = T(0);
    result.left_match.assign(static_cast<std::size_t>(left_size), -1);
    result.right_match.assign(static_cast<std::size_t>(right_size), -1);
    std::vector<int> parent_vertex(static_cast<std::size_t>(vertex_count));
    std::vector<int> parent_edge(static_cast<std::size_t>(vertex_count));
    using QueueEntry = std::pair<T, int>;
    while(true){
        std::fill(distance.begin(), distance.end(), inf);
        std::fill(reached.begin(), reached.end(), 0);
        std::fill(parent_vertex.begin(), parent_vertex.end(), -1);
        std::fill(parent_edge.begin(), parent_edge.end(), -1);
        std::priority_queue<
            QueueEntry,
            std::vector<QueueEntry>,
            std::greater<QueueEntry>
        > queue;
        distance[static_cast<std::size_t>(source)] = T(0);
        reached[static_cast<std::size_t>(source)] = 1;
        queue.push({T(0), source});
        while(!queue.empty()){
            const auto [current_distance, vertex] = queue.top();
            queue.pop();
            if(distance[static_cast<std::size_t>(vertex)] < current_distance){
                continue;
            }
            const auto& adjacent = graph[static_cast<std::size_t>(vertex)];
            for(int edge_index = 0;
                edge_index < static_cast<int>(adjacent.size());
                ++edge_index){
                const Edge& edge =
                    adjacent[static_cast<std::size_t>(edge_index)];
                if(edge.cap == 0) continue;
                const T reduced_cost = edge.cost
                    + potential[static_cast<std::size_t>(vertex)]
                    - potential[static_cast<std::size_t>(edge.to)];
                const T candidate = current_distance + reduced_cost;
                if(reached[static_cast<std::size_t>(edge.to)]
                    && !(candidate < distance[static_cast<std::size_t>(edge.to)])){
                    continue;
                }
                distance[static_cast<std::size_t>(edge.to)] = candidate;
                reached[static_cast<std::size_t>(edge.to)] = 1;
                parent_vertex[static_cast<std::size_t>(edge.to)] = vertex;
                parent_edge[static_cast<std::size_t>(edge.to)] = edge_index;
                queue.push({candidate, edge.to});
            }
        }
        if(!reached[static_cast<std::size_t>(sink)]) break;
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            if(reached[static_cast<std::size_t>(vertex)]){
                potential[static_cast<std::size_t>(vertex)] +=
                    distance[static_cast<std::size_t>(vertex)];
            }
        }
        for(int vertex = sink; vertex != source;
            vertex = parent_vertex[static_cast<std::size_t>(vertex)]){
            const int previous =
                parent_vertex[static_cast<std::size_t>(vertex)];
            Edge& edge = graph[static_cast<std::size_t>(previous)]
                [static_cast<std::size_t>(
                    parent_edge[static_cast<std::size_t>(vertex)]
                )];
            --edge.cap;
            ++graph[static_cast<std::size_t>(edge.to)]
                [static_cast<std::size_t>(edge.reverse)].cap;
        }
        ++result.size;
        result.cost += potential[static_cast<std::size_t>(sink)]
            - potential[static_cast<std::size_t>(source)];
    }

    for(int left = 0; left < left_size; ++left){
        for(const Edge& edge: graph[static_cast<std::size_t>(left)]){
            if(edge.input_id < 0 || edge.cap != 0) continue;
            const int right = edge.to - left_size;
            result.left_match[static_cast<std::size_t>(left)] = right;
            result.right_match[static_cast<std::size_t>(right)] = left;
        }
    }
    return result;
}
