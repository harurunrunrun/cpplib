#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MINIMUM_PATH_COVER_DAG_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MINIMUM_PATH_COVER_DAG_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

#include "../../graph/traversal/topological_sort.hpp"
#include "bipartite_matching.hpp"

struct MinimumPathCoverDagResult{
    int count;
    std::vector<std::vector<int>> paths;
    std::vector<int> successor;
    std::vector<int> predecessor;
};

inline MinimumPathCoverDagResult minimum_path_cover_dag(
    const std::vector<std::vector<int>>& graph
){
    const int vertex_count = static_cast<int>(graph.size());
    TopologicalSortResult topological = topological_sort(graph);
    if(!topological.is_dag)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: graph is not a DAG (minimum_path_cover_dag)."
        );
    }
    BipartiteMatching matching(vertex_count, vertex_count);
    for(int from = 0; from < vertex_count; ++from){
        for(int to: graph[static_cast<std::size_t>(from)]){
            matching.add_edge(from, to);
        }
    }
    BipartiteMatchingResult matched = matching.solve();
    std::vector<int> successor = std::move(matched.left_match);
    std::vector<int> predecessor = std::move(matched.right_match);
    std::vector<std::vector<int>> paths;
    paths.reserve(static_cast<std::size_t>(vertex_count - matched.size));
    for(int start = 0; start < vertex_count; ++start){
        if(predecessor[static_cast<std::size_t>(start)] != -1) continue;
        std::vector<int> path;
        for(int vertex = start; vertex != -1;
            vertex = successor[static_cast<std::size_t>(vertex)]){
            path.push_back(vertex);
        }
        paths.push_back(std::move(path));
    }
    return {
        vertex_count - matched.size,
        std::move(paths),
        std::move(successor),
        std::move(predecessor)
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_MINIMUM_PATH_COVER_DAG_HPP_INCLUDED
