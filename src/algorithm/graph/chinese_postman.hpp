#pragma once

#include <concepts>
#include <cstddef>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <set>
#include <utility>
#include <vector>

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include "../matching/minimum_weight_general_matching.hpp"
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

template<std::signed_integral T>
struct ChinesePostmanEdge{
    int from;
    int to;
    T cost;
};

template<std::signed_integral T>
struct ChinesePostmanResult{
    T cost;
    std::vector<std::pair<int, int>> paired_odd_vertices;
};

template<std::signed_integral T>
std::optional<ChinesePostmanResult<T>> undirected_chinese_postman(
    int vertex_count,
    const std::vector<ChinesePostmanEdge<T>>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::invalid_argument("Chinese postman vertex count must be nonnegative");
    }
    using Wide = minimum_weight_general_matching_internal::Wide;
    constexpr Wide infinity = Wide(1) << 120;
    struct AdjacencyEdge{
        int to;
        Wide cost;
    };
    std::vector<std::vector<AdjacencyEdge>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    std::vector<int> degree(static_cast<std::size_t>(vertex_count));

    Wide base_cost = 0;
    for(const auto& edge: edges){
        if(edge.from < 0 || vertex_count <= edge.from
           || edge.to < 0 || vertex_count <= edge.to)[[unlikely]]{
            throw std::invalid_argument("Chinese postman edge endpoint is outside the graph");
        }
        if(edge.cost < 0)[[unlikely]]{
            throw std::invalid_argument("Chinese postman edge cost must be nonnegative");
        }
        base_cost += static_cast<Wide>(edge.cost);
        ++degree[static_cast<std::size_t>(edge.from)];
        ++degree[static_cast<std::size_t>(edge.to)];
        const Wide cost = static_cast<Wide>(edge.cost);
        graph[static_cast<std::size_t>(edge.from)].push_back({edge.to, cost});
        graph[static_cast<std::size_t>(edge.to)].push_back({edge.from, cost});
    }

    int start = -1;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(degree[static_cast<std::size_t>(vertex)] != 0){
            start = vertex;
            break;
        }
    }
    if(start != -1){
        std::vector<char> visited(static_cast<std::size_t>(vertex_count));
        std::queue<int> queue;
        visited[static_cast<std::size_t>(start)] = 1;
        queue.push(start);
        while(!queue.empty()){
            const int vertex = queue.front();
            queue.pop();
            for(const auto& edge: graph[static_cast<std::size_t>(vertex)]){
                if(!visited[static_cast<std::size_t>(edge.to)]){
                    visited[static_cast<std::size_t>(edge.to)] = 1;
                    queue.push(edge.to);
                }
            }
        }
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            if(degree[static_cast<std::size_t>(vertex)] != 0
               && !visited[static_cast<std::size_t>(vertex)]){
                return std::nullopt;
            }
        }
    }

    std::vector<int> odd_vertices;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(degree[static_cast<std::size_t>(vertex)] & 1){
            odd_vertices.push_back(vertex);
        }
    }
    const int odd_count = static_cast<int>(odd_vertices.size());
    std::vector<std::vector<Wide>> odd_distance(
        static_cast<std::size_t>(odd_count),
        std::vector<Wide>(static_cast<std::size_t>(odd_count), infinity)
    );
    std::vector<int> odd_index(static_cast<std::size_t>(vertex_count), -1);
    for(int index = 0; index < odd_count; ++index){
        odd_index[static_cast<std::size_t>(
            odd_vertices[static_cast<std::size_t>(index)]
        )] = index;
    }
    using QueueEntry = std::pair<Wide, int>;
    for(int source_index = 0; source_index < odd_count; ++source_index){
        const int source = odd_vertices[static_cast<std::size_t>(source_index)];
        std::vector<Wide> distance(static_cast<std::size_t>(vertex_count), infinity);
        std::set<QueueEntry> heap;
        distance[static_cast<std::size_t>(source)] = 0;
        heap.insert({0, source});
        int remaining = odd_count;
        while(!heap.empty() && remaining != 0){
            const auto iterator = heap.begin();
            const auto [current_distance, vertex] = *iterator;
            heap.erase(iterator);
            const int target_index = odd_index[static_cast<std::size_t>(vertex)];
            if(target_index != -1){
                odd_distance[static_cast<std::size_t>(source_index)]
                            [static_cast<std::size_t>(target_index)] = current_distance;
                --remaining;
            }
            for(const auto& edge: graph[static_cast<std::size_t>(vertex)]){
                const Wide candidate = current_distance <= infinity - edge.cost
                                     ? current_distance + edge.cost
                                     : infinity;
                Wide& next_distance = distance[static_cast<std::size_t>(edge.to)];
                if(candidate < next_distance){
                    if(next_distance != infinity){
                        heap.erase({next_distance, edge.to});
                    }
                    next_distance = candidate;
                    heap.insert({candidate, edge.to});
                }
            }
        }
    }
    std::vector<MinimumWeightGeneralMatchingEdge<T>> matching_edges;
    const Wide maximum = static_cast<Wide>(std::numeric_limits<T>::max());
    for(int left = 0; left < odd_count; ++left){
        for(int right = left + 1; right < odd_count; ++right){
            const Wide cost = odd_distance[static_cast<std::size_t>(left)]
                                          [static_cast<std::size_t>(right)];
            if(cost <= maximum){
                matching_edges.push_back({left, right, static_cast<T>(cost)});
            }
        }
    }
    const auto matching = minimum_weight_general_matching<T>(
        odd_count, matching_edges
    );
    if(matching.size * 2 != odd_count)[[unlikely]]{
        throw std::overflow_error("Chinese postman shortest-path cost exceeds the value type");
    }

    const Wide total = base_cost + static_cast<Wide>(matching.cost);
    if(maximum < total)[[unlikely]]{
        throw std::overflow_error("Chinese postman cost exceeds the value type");
    }
    ChinesePostmanResult<T> result{static_cast<T>(total), {}};
    result.paired_odd_vertices.reserve(odd_vertices.size() / 2);
    for(int vertex = 0; vertex < odd_count; ++vertex){
        const int other = matching.match[static_cast<std::size_t>(vertex)];
        if(vertex < other){
            result.paired_odd_vertices.push_back({
                odd_vertices[static_cast<std::size_t>(vertex)],
                odd_vertices[static_cast<std::size_t>(other)],
            });
        }
    }
    return result;
}
