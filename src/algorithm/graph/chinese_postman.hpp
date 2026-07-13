#pragma once

#include <concepts>
#include <cstddef>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
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
    std::vector<std::vector<Wide>> distance(
        static_cast<std::size_t>(vertex_count),
        std::vector<Wide>(static_cast<std::size_t>(vertex_count), infinity)
    );
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(vertex_count));
    std::vector<int> degree(static_cast<std::size_t>(vertex_count));
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        distance[static_cast<std::size_t>(vertex)][static_cast<std::size_t>(vertex)] = 0;
    }

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
        graph[static_cast<std::size_t>(edge.from)].push_back(edge.to);
        graph[static_cast<std::size_t>(edge.to)].push_back(edge.from);
        Wide& forward = distance[static_cast<std::size_t>(edge.from)]
                                [static_cast<std::size_t>(edge.to)];
        Wide& backward = distance[static_cast<std::size_t>(edge.to)]
                                 [static_cast<std::size_t>(edge.from)];
        const Wide cost = static_cast<Wide>(edge.cost);
        if(cost < forward) forward = backward = cost;
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
            for(int next: graph[static_cast<std::size_t>(vertex)]){
                if(!visited[static_cast<std::size_t>(next)]){
                    visited[static_cast<std::size_t>(next)] = 1;
                    queue.push(next);
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

    for(int middle = 0; middle < vertex_count; ++middle){
        for(int from = 0; from < vertex_count; ++from){
            const Wide left = distance[static_cast<std::size_t>(from)]
                                      [static_cast<std::size_t>(middle)];
            if(left == infinity) continue;
            for(int to = 0; to < vertex_count; ++to){
                const Wide right = distance[static_cast<std::size_t>(middle)]
                                           [static_cast<std::size_t>(to)];
                if(right == infinity) continue;
                Wide& current = distance[static_cast<std::size_t>(from)]
                                        [static_cast<std::size_t>(to)];
                if(left + right < current) current = left + right;
            }
        }
    }

    std::vector<int> odd_vertices;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(degree[static_cast<std::size_t>(vertex)] & 1){
            odd_vertices.push_back(vertex);
        }
    }
    std::vector<MinimumWeightGeneralMatchingEdge<T>> matching_edges;
    const Wide maximum = static_cast<Wide>(std::numeric_limits<T>::max());
    for(int left = 0; left < static_cast<int>(odd_vertices.size()); ++left){
        for(int right = left + 1; right < static_cast<int>(odd_vertices.size()); ++right){
            const Wide cost = distance[static_cast<std::size_t>(odd_vertices[left])]
                                      [static_cast<std::size_t>(odd_vertices[right])];
            if(cost <= maximum){
                matching_edges.push_back({left, right, static_cast<T>(cost)});
            }
        }
    }
    const auto matching = minimum_weight_general_matching<T>(
        static_cast<int>(odd_vertices.size()), matching_edges
    );
    if(matching.size * 2 != static_cast<int>(odd_vertices.size()))[[unlikely]]{
        throw std::overflow_error("Chinese postman shortest-path cost exceeds the value type");
    }

    const Wide total = base_cost + static_cast<Wide>(matching.cost);
    if(maximum < total)[[unlikely]]{
        throw std::overflow_error("Chinese postman cost exceeds the value type");
    }
    ChinesePostmanResult<T> result{static_cast<T>(total), {}};
    result.paired_odd_vertices.reserve(odd_vertices.size() / 2);
    for(int vertex = 0; vertex < static_cast<int>(odd_vertices.size()); ++vertex){
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
