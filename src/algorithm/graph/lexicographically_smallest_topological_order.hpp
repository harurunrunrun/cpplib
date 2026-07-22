#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_LEXICOGRAPHICALLY_SMALLEST_TOPOLOGICAL_ORDER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_LEXICOGRAPHICALLY_SMALLEST_TOPOLOGICAL_ORDER_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <optional>
#include <utility>
#include <vector>

#include "topological_sort.hpp"

inline std::optional<std::vector<int>>
lexicographically_smallest_topological_order(
    const std::vector<std::vector<int>>& graph
){
    if(graph.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error("topological graph is too large");
    }
    auto result = lexicographical_topological_sort(graph);
    if(!result.is_dag) return std::nullopt;
    return std::move(result.order);
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_LEXICOGRAPHICALLY_SMALLEST_TOPOLOGICAL_ORDER_HPP_INCLUDED
