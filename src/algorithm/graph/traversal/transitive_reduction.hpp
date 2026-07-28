#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_TRANSITIVE_REDUCTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_TRANSITIVE_REDUCTION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

#include "topological_sort.hpp"

inline std::vector<std::pair<int, int>> transitive_reduction(
    const std::vector<std::vector<int>>& graph
){
    const int vertex_count = static_cast<int>(graph.size());
    TopologicalSortResult topological = topological_sort(graph);
    if(!topological.is_dag)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: graph is not a DAG (transitive_reduction)."
        );
    }
    std::vector<int> position(static_cast<std::size_t>(vertex_count));
    for(int index = 0; index < vertex_count; ++index){
        position[static_cast<std::size_t>(
            topological.order[static_cast<std::size_t>(index)]
        )] = index;
    }
    const std::size_t word_count =
        (static_cast<std::size_t>(vertex_count) + 63U) / 64U;
    std::vector<std::vector<std::uint64_t>> reachable(
        static_cast<std::size_t>(vertex_count),
        std::vector<std::uint64_t>(word_count, 0)
    );
    std::vector<std::pair<int, int>> result;
    for(int order_index = vertex_count - 1; order_index >= 0; --order_index){
        int vertex = topological.order[static_cast<std::size_t>(order_index)];
        std::vector<int> next = graph[static_cast<std::size_t>(vertex)];
        std::sort(next.begin(), next.end(), [&](int left, int right){
            return position[static_cast<std::size_t>(left)]
                < position[static_cast<std::size_t>(right)];
        });
        next.erase(std::unique(next.begin(), next.end()), next.end());
        auto& covered = reachable[static_cast<std::size_t>(vertex)];
        for(int to: next){
            std::size_t word = static_cast<std::size_t>(to) >> 6U;
            std::uint64_t bit = std::uint64_t{1}
                << (static_cast<unsigned>(to) & 63U);
            if((covered[word] & bit) != 0) continue;
            result.emplace_back(vertex, to);
            covered[word] |= bit;
            const auto& suffix = reachable[static_cast<std::size_t>(to)];
            for(std::size_t index = 0; index < word_count; ++index){
                covered[index] |= suffix[index];
            }
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_TRANSITIVE_REDUCTION_HPP_INCLUDED
