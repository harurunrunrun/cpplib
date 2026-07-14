#pragma once

#include <cstddef>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <vector>

#include "iterative_strongly_connected_components.hpp"

inline std::optional<long long> maximum_fun_walk(
    const std::vector<std::vector<int>>& graph,
    const std::vector<long long>& fun,
    int start,
    int goal
){
    if(graph.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error("maximum_fun_walk graph is too large");
    }
    const int size = static_cast<int>(graph.size());
    if(fun.size() != graph.size())[[unlikely]]{
        throw std::invalid_argument("maximum_fun_walk size mismatch");
    }
    if(start < 0 || size <= start || goal < 0 || size <= goal)[[unlikely]]{
        throw std::out_of_range("maximum_fun_walk endpoint is out of range");
    }
    const auto components = iterative_strongly_connected_components(graph);
    std::vector<long long> weight(static_cast<std::size_t>(components.count));
    for(int vertex = 0; vertex < size; ++vertex){
        const int component = components.id[static_cast<std::size_t>(vertex)];
        const long long value = fun[static_cast<std::size_t>(vertex)];
        if(value < 0)[[unlikely]]{
            throw std::invalid_argument("maximum_fun_walk negative fun value");
        }
        if(weight[static_cast<std::size_t>(component)] >
            std::numeric_limits<long long>::max() - value)[[unlikely]]{
            throw std::overflow_error("maximum_fun_walk component sum overflow");
        }
        weight[static_cast<std::size_t>(component)] += value;
    }

    std::vector<std::vector<int>> dag(
        static_cast<std::size_t>(components.count)
    );
    std::vector<int> indegree(static_cast<std::size_t>(components.count));
    for(int vertex = 0; vertex < size; ++vertex){
        const int from = components.id[static_cast<std::size_t>(vertex)];
        for(const int to_vertex: graph[static_cast<std::size_t>(vertex)]){
            const int to = components.id[static_cast<std::size_t>(to_vertex)];
            if(from != to){
                dag[static_cast<std::size_t>(from)].push_back(to);
                ++indegree[static_cast<std::size_t>(to)];
            }
        }
    }
    std::queue<int> queue;
    for(int component = 0; component < components.count; ++component){
        if(indegree[static_cast<std::size_t>(component)] == 0){
            queue.push(component);
        }
    }
    constexpr long long UNREACHABLE = std::numeric_limits<long long>::min();
    std::vector<long long> best(
        static_cast<std::size_t>(components.count), UNREACHABLE
    );
    const int start_component = components.id[static_cast<std::size_t>(start)];
    best[static_cast<std::size_t>(start_component)] =
        weight[static_cast<std::size_t>(start_component)];
    while(!queue.empty()){
        const int from = queue.front();
        queue.pop();
        for(const int to: dag[static_cast<std::size_t>(from)]){
            if(best[static_cast<std::size_t>(from)] != UNREACHABLE){
                const long long add = weight[static_cast<std::size_t>(to)];
                if(best[static_cast<std::size_t>(from)] >
                    std::numeric_limits<long long>::max() - add)[[unlikely]]{
                    throw std::overflow_error("maximum_fun_walk path sum overflow");
                }
                const long long candidate =
                    best[static_cast<std::size_t>(from)] + add;
                if(best[static_cast<std::size_t>(to)] < candidate){
                    best[static_cast<std::size_t>(to)] = candidate;
                }
            }
            if(--indegree[static_cast<std::size_t>(to)] == 0){
                queue.push(to);
            }
        }
    }
    const long long answer = best[static_cast<std::size_t>(
        components.id[static_cast<std::size_t>(goal)]
    )];
    if(answer == UNREACHABLE) return std::nullopt;
    return answer;
}
