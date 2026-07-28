#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MAXIMUM_ANTICHAIN_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MAXIMUM_ANTICHAIN_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <queue>
#include <stdexcept>
#include <vector>

#include "../../graph/traversal/topological_sort.hpp"
#include "bipartite_matching.hpp"

struct MaximumAntichainResult{
    int size;
    std::vector<int> vertices;
};

inline MaximumAntichainResult maximum_antichain(
    const std::vector<std::vector<int>>& graph
){
    const int vertex_count = static_cast<int>(graph.size());
    TopologicalSortResult topological = topological_sort(graph);
    if(!topological.is_dag)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: graph is not a DAG (maximum_antichain)."
        );
    }
    const std::size_t word_count =
        (static_cast<std::size_t>(vertex_count) + 63U) / 64U;
    std::vector<std::vector<std::uint64_t>> reachable(
        static_cast<std::size_t>(vertex_count),
        std::vector<std::uint64_t>(word_count, 0)
    );
    for(int index = vertex_count - 1; index >= 0; --index){
        int vertex = topological.order[static_cast<std::size_t>(index)];
        auto& current = reachable[static_cast<std::size_t>(vertex)];
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            current[static_cast<std::size_t>(to) >> 6U] |=
                std::uint64_t{1} << (static_cast<unsigned>(to) & 63U);
            const auto& suffix = reachable[static_cast<std::size_t>(to)];
            for(std::size_t word = 0; word < word_count; ++word){
                current[word] |= suffix[word];
            }
        }
    }

    BipartiteMatching matching(vertex_count, vertex_count);
    for(int left = 0; left < vertex_count; ++left){
        const auto& row = reachable[static_cast<std::size_t>(left)];
        for(int right = 0; right < vertex_count; ++right){
            if(((row[static_cast<std::size_t>(right) >> 6U]
                >> (static_cast<unsigned>(right) & 63U)) & 1U) != 0){
                matching.add_edge(left, right);
            }
        }
    }
    BipartiteMatchingResult matched = matching.solve();
    std::vector<unsigned char> seen_left(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::vector<unsigned char> seen_right(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::queue<int> queue;
    for(int left = 0; left < vertex_count; ++left){
        if(matched.left_match[static_cast<std::size_t>(left)] == -1){
            seen_left[static_cast<std::size_t>(left)] = 1;
            queue.push(left);
        }
    }
    while(!queue.empty()){
        int left = queue.front();
        queue.pop();
        for(int right: matching.graph[static_cast<std::size_t>(left)]){
            if(matched.left_match[static_cast<std::size_t>(left)] == right){
                continue;
            }
            if(seen_right[static_cast<std::size_t>(right)] != 0) continue;
            seen_right[static_cast<std::size_t>(right)] = 1;
            int next_left =
                matched.right_match[static_cast<std::size_t>(right)];
            if(next_left != -1
                && seen_left[static_cast<std::size_t>(next_left)] == 0){
                seen_left[static_cast<std::size_t>(next_left)] = 1;
                queue.push(next_left);
            }
        }
    }
    std::vector<int> vertices;
    vertices.reserve(
        static_cast<std::size_t>(vertex_count - matched.size)
    );
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(seen_left[static_cast<std::size_t>(vertex)] != 0
            && seen_right[static_cast<std::size_t>(vertex)] == 0){
            vertices.push_back(vertex);
        }
    }
    return {static_cast<int>(vertices.size()), std::move(vertices)};
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MAXIMUM_ANTICHAIN_HPP_INCLUDED
