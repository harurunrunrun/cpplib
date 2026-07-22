#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_HIERARCHY_FROM_SUPERIOR_CONSTRAINTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_HIERARCHY_FROM_SUPERIOR_CONSTRAINTS_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "topological_sort.hpp"

inline std::vector<int> hierarchy_from_superior_constraints(
    const std::vector<std::vector<int>>& constraints
){
    if(constraints.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error("superior constraint graph is too large");
    }
    const auto sorted = topological_sort(constraints);
    if(!sorted.is_dag)[[unlikely]]{
        throw std::invalid_argument("superior constraints contain a cycle");
    }
    std::vector<int> parent(constraints.size(), -1);
    for(std::size_t index = 1; index < sorted.order.size(); ++index){
        parent[static_cast<std::size_t>(sorted.order[index])] =
            sorted.order[index - 1];
    }
    return parent;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_HIERARCHY_FROM_SUPERIOR_CONSTRAINTS_HPP_INCLUDED
