#pragma once

#include <cstddef>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "bipartite_matching.hpp"

struct BipartiteVertexSet{
    int size = 0;
    std::vector<char> left;
    std::vector<char> right;
};

struct BipartiteKonigDecomposition{
    BipartiteVertexSet minimum_vertex_cover;
    BipartiteVertexSet maximum_independent_set;
};

namespace bipartite_cover_internal{

inline void validate_matching(
    const BipartiteMatching& graph,
    const BipartiteMatchingResult& matching
){
    if(matching.left_match.size() != static_cast<std::size_t>(graph.left_size)
       || matching.right_match.size() != static_cast<std::size_t>(graph.right_size)){
        throw std::invalid_argument("bipartite matching dimensions do not match the graph");
    }
    int matched = 0;
    for(int left = 0; left < graph.left_size; ++left){
        const int right = matching.left_match[static_cast<std::size_t>(left)];
        if(right == -1) continue;
        if(right < 0 || graph.right_size <= right
           || matching.right_match[static_cast<std::size_t>(right)] != left){
            throw std::invalid_argument("inconsistent bipartite matching");
        }
        bool exists = false;
        for(int endpoint: graph.graph[static_cast<std::size_t>(left)]){
            if(endpoint == right){
                exists = true;
                break;
            }
        }
        if(!exists){
            throw std::invalid_argument("a matched pair is not an edge");
        }
        ++matched;
    }
    for(int right = 0; right < graph.right_size; ++right){
        const int left = matching.right_match[static_cast<std::size_t>(right)];
        if(left != -1
           && (left < 0 || graph.left_size <= left
               || matching.left_match[static_cast<std::size_t>(left)] != right)){
            throw std::invalid_argument("inconsistent bipartite matching");
        }
    }
    if(matched != matching.size){
        throw std::invalid_argument("bipartite matching size is inconsistent");
    }
}

} // namespace bipartite_cover_internal

inline BipartiteKonigDecomposition bipartite_konig_decomposition(
    const BipartiteMatching& graph,
    const BipartiteMatchingResult& maximum_matching
){
    bipartite_cover_internal::validate_matching(graph, maximum_matching);

    std::vector<char> reachable_left(static_cast<std::size_t>(graph.left_size));
    std::vector<char> reachable_right(static_cast<std::size_t>(graph.right_size));
    std::queue<int> queue;
    for(int left = 0; left < graph.left_size; ++left){
        if(maximum_matching.left_match[static_cast<std::size_t>(left)] == -1){
            reachable_left[static_cast<std::size_t>(left)] = 1;
            queue.push(left);
        }
    }
    while(!queue.empty()){
        const int left = queue.front();
        queue.pop();
        for(int right: graph.graph[static_cast<std::size_t>(left)]){
            if(maximum_matching.left_match[static_cast<std::size_t>(left)] == right){
                continue;
            }
            if(reachable_right[static_cast<std::size_t>(right)]) continue;
            reachable_right[static_cast<std::size_t>(right)] = 1;
            const int next_left = maximum_matching.right_match[static_cast<std::size_t>(right)];
            if(next_left == -1){
                throw std::invalid_argument("the supplied bipartite matching is not maximum");
            }
            if(!reachable_left[static_cast<std::size_t>(next_left)]){
                reachable_left[static_cast<std::size_t>(next_left)] = 1;
                queue.push(next_left);
            }
        }
    }

    BipartiteKonigDecomposition result;
    result.minimum_vertex_cover.left.resize(static_cast<std::size_t>(graph.left_size));
    result.minimum_vertex_cover.right.resize(static_cast<std::size_t>(graph.right_size));
    result.maximum_independent_set.left.resize(static_cast<std::size_t>(graph.left_size));
    result.maximum_independent_set.right.resize(static_cast<std::size_t>(graph.right_size));
    for(int left = 0; left < graph.left_size; ++left){
        const bool in_cover = !reachable_left[static_cast<std::size_t>(left)];
        result.minimum_vertex_cover.left[static_cast<std::size_t>(left)] = in_cover;
        result.maximum_independent_set.left[static_cast<std::size_t>(left)] = !in_cover;
        result.minimum_vertex_cover.size += in_cover;
        result.maximum_independent_set.size += !in_cover;
    }
    for(int right = 0; right < graph.right_size; ++right){
        const bool in_cover = reachable_right[static_cast<std::size_t>(right)];
        result.minimum_vertex_cover.right[static_cast<std::size_t>(right)] = in_cover;
        result.maximum_independent_set.right[static_cast<std::size_t>(right)] = !in_cover;
        result.minimum_vertex_cover.size += in_cover;
        result.maximum_independent_set.size += !in_cover;
    }
    return result;
}

inline std::optional<std::vector<std::pair<int, int>>> minimum_bipartite_edge_cover(
    const BipartiteMatching& graph,
    const BipartiteMatchingResult& maximum_matching
){
    (void)bipartite_konig_decomposition(graph, maximum_matching);

    std::vector<int> right_incident(static_cast<std::size_t>(graph.right_size), -1);
    for(int left = 0; left < graph.left_size; ++left){
        for(int right: graph.graph[static_cast<std::size_t>(left)]){
            right_incident[static_cast<std::size_t>(right)] = left;
        }
    }
    for(int left = 0; left < graph.left_size; ++left){
        if(graph.graph[static_cast<std::size_t>(left)].empty()) return std::nullopt;
    }
    for(int right = 0; right < graph.right_size; ++right){
        if(right_incident[static_cast<std::size_t>(right)] == -1) return std::nullopt;
    }

    std::vector<std::pair<int, int>> result;
    result.reserve(static_cast<std::size_t>(
        graph.left_size + graph.right_size - maximum_matching.size
    ));
    for(int left = 0; left < graph.left_size; ++left){
        const int right = maximum_matching.left_match[static_cast<std::size_t>(left)];
        if(right != -1) result.push_back({left, right});
    }
    for(int left = 0; left < graph.left_size; ++left){
        if(maximum_matching.left_match[static_cast<std::size_t>(left)] == -1){
            result.push_back({left, graph.graph[static_cast<std::size_t>(left)].front()});
        }
    }
    for(int right = 0; right < graph.right_size; ++right){
        if(maximum_matching.right_match[static_cast<std::size_t>(right)] == -1){
            result.push_back({right_incident[static_cast<std::size_t>(right)], right});
        }
    }
    return result;
}
