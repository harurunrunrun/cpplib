#ifndef CPPLIB_SRC_APPROXIMATE_RANDOMIZED_COLOR_CODING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_RANDOMIZED_COLOR_CODING_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

namespace approximate::randomized {

struct ColorCodingPathResult {
    std::vector<std::size_t> path;
    std::size_t trials = 0;
    bool found = false;
};

namespace detail {

inline void validate_color_coding_graph(
    const std::vector<std::vector<std::size_t>>& graph
){
    for(const auto& edges: graph){
        for(const std::size_t to: edges){
            if(to >= graph.size())[[unlikely]]{
                throw std::invalid_argument("an adjacency endpoint is out of range");
            }
        }
    }
}

}  // namespace detail

[[nodiscard]] inline bool is_valid_color_coding_path(
    const std::vector<std::vector<std::size_t>>& graph,
    std::size_t vertex_count,
    const ColorCodingPathResult& result
){
    for(const auto& edges: graph){
        for(const std::size_t to: edges){
            if(to >= graph.size()) return false;
        }
    }
    if(!result.found) return result.path.empty();
    if(result.path.size() != vertex_count) return false;
    std::vector<unsigned char> used(graph.size(), 0);
    for(const std::size_t vertex: result.path){
        if(vertex >= graph.size() || used[vertex] != 0) return false;
        used[vertex] = 1;
    }
    for(std::size_t index = 1; index < result.path.size(); ++index){
        const auto& edges = graph[result.path[index - 1]];
        if(std::find(edges.begin(), edges.end(), result.path[index]) == edges.end()){
            return false;
        }
    }
    return true;
}

inline void validate_color_coding_path(
    const std::vector<std::vector<std::size_t>>& graph,
    std::size_t vertex_count,
    const ColorCodingPathResult& result
){
    if(!is_valid_color_coding_path(graph, vertex_count, result)){
        throw std::invalid_argument("the color-coding path result is inconsistent");
    }
}

template<class UniformRandomBitGenerator>
[[nodiscard]] ColorCodingPathResult color_coding_simple_path(
    const std::vector<std::vector<std::size_t>>& graph,
    std::size_t vertex_count,
    std::size_t trial_limit,
    UniformRandomBitGenerator& generator
){
    detail::validate_color_coding_graph(graph);
    ColorCodingPathResult result;
    if(vertex_count == 0){
        result.found = true;
        return result;
    }
    const std::size_t n = graph.size();
    if(vertex_count > n) return result;
    if(vertex_count == 1){
        result.path.push_back(0);
        result.found = true;
        return result;
    }
    if(vertex_count >= std::numeric_limits<std::size_t>::digits)[[unlikely]]{
        throw std::length_error("the color count does not fit in a subset mask");
    }
    const std::size_t state_count = std::size_t{1} << vertex_count;
    const std::size_t maximum = std::vector<std::size_t>{}.max_size();
    if(n != 0 && state_count > maximum / n)[[unlikely]]{
        throw std::length_error("the color-coding table is too large");
    }
    const std::size_t table_size = state_count * n;
    constexpr std::size_t unreachable = std::numeric_limits<std::size_t>::max();
    const std::size_t start_marker = n;
    std::uniform_int_distribution<std::size_t> choose_color(0, vertex_count - 1);
    std::vector<std::size_t> colors(n);
    std::vector<std::size_t> predecessor;
    predecessor.reserve(table_size);
    const std::size_t full_mask = state_count - 1;

    std::size_t completed_trials = 0;
    while(completed_trials < trial_limit){
        ++completed_trials;
        result.trials = completed_trials;
        for(std::size_t& color: colors) color = choose_color(generator);
        predecessor.assign(table_size, unreachable);
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            const std::size_t mask = std::size_t{1} << colors[vertex];
            predecessor[mask * n + vertex] = start_marker;
        }
        for(std::size_t mask = 1; mask <= full_mask; ++mask){
            for(std::size_t vertex = 0; vertex < n; ++vertex){
                if(predecessor[mask * n + vertex] == unreachable) continue;
                for(const std::size_t to: graph[vertex]){
                    const std::size_t bit = std::size_t{1} << colors[to];
                    if((mask & bit) != 0) continue;
                    const std::size_t next_mask = mask | bit;
                    std::size_t& next = predecessor[next_mask * n + to];
                    if(next == unreachable) next = vertex;
                }
            }
        }
        std::size_t last = n;
        for(std::size_t vertex = 0; vertex < n; ++vertex){
            if(predecessor[full_mask * n + vertex] != unreachable){
                last = vertex;
                break;
            }
        }
        if(last == n) continue;

        std::vector<std::size_t> path;
        path.reserve(vertex_count);
        std::size_t mask = full_mask;
        std::size_t current = last;
        while(true){
            path.push_back(current);
            const std::size_t previous = predecessor[mask * n + current];
            if(previous == start_marker) break;
            mask ^= std::size_t{1} << colors[current];
            current = previous;
        }
        std::reverse(path.begin(), path.end());
        result.path = std::move(path);
        result.found = true;
        return result;
    }
    return result;
}

}  // namespace approximate::randomized

#endif  // CPPLIB_SRC_APPROXIMATE_RANDOMIZED_COLOR_CODING_HPP_INCLUDED
