#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_INCREMENTAL_SCC_PAIR_PRODUCT_SUMS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_INCREMENTAL_SCC_PAIR_PRODUCT_SUMS_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "incremental_scc.hpp"

template<class T>
std::vector<T> incremental_scc_pair_product_sums(
    const std::vector<T>& vertex_weights,
    const std::vector<std::pair<int, int>>& added_edges
){
    if(vertex_weights.size() >=
       static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
       added_edges.size() >=
       static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
        throw std::length_error(
            "incremental_scc_pair_product_sums input is too large"
        );
    }
    const int vertex_count = static_cast<int>(vertex_weights.size());
    const int edge_count = static_cast<int>(added_edges.size());
    const std::vector<int> merge_time = incremental_scc(
        vertex_count, added_edges
    );

    std::vector<std::vector<int>> events(
        static_cast<std::size_t>(edge_count + 1)
    );
    for(int edge = 0; edge < edge_count; ++edge){
        const int time = merge_time[static_cast<std::size_t>(edge)];
        if(time <= edge_count){
            events[static_cast<std::size_t>(time)].push_back(edge);
        }
    }

    std::vector<int> parent_or_size(
        static_cast<std::size_t>(vertex_count), -1
    );
    const auto leader = [&](auto&& self, int vertex) -> int{
        int& parent = parent_or_size[static_cast<std::size_t>(vertex)];
        if(parent < 0) return vertex;
        return parent = self(self, parent);
    };
    std::vector<T> component_weight = vertex_weights;
    std::vector<T> result;
    result.reserve(added_edges.size());
    T answer{};

    for(int time = 1; time <= edge_count; ++time){
        for(int edge: events[static_cast<std::size_t>(time)]){
            int left = leader(
                leader, added_edges[static_cast<std::size_t>(edge)].first
            );
            int right = leader(
                leader, added_edges[static_cast<std::size_t>(edge)].second
            );
            if(left == right) continue;
            if(parent_or_size[static_cast<std::size_t>(left)] >
               parent_or_size[static_cast<std::size_t>(right)]){
                std::swap(left, right);
            }
            answer += component_weight[static_cast<std::size_t>(left)] *
                component_weight[static_cast<std::size_t>(right)];
            component_weight[static_cast<std::size_t>(left)] +=
                component_weight[static_cast<std::size_t>(right)];
            parent_or_size[static_cast<std::size_t>(left)] +=
                parent_or_size[static_cast<std::size_t>(right)];
            parent_or_size[static_cast<std::size_t>(right)] = left;
        }
        result.push_back(answer);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_INCREMENTAL_SCC_PAIR_PRODUCT_SUMS_HPP_INCLUDED
