#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_DEGENERACY_ORDERING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_DEGENERACY_ORDERING_HPP_INCLUDED

#include <utility>
#include <vector>

#include "../connectivity/decomposition/core_decomposition.hpp"

struct DegeneracyOrderingResult{
    std::vector<int> order;
    int degeneracy;
};

inline DegeneracyOrderingResult degeneracy_ordering(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    CoreDecompositionResult result = core_decomposition(vertex_count, edges);
    return {std::move(result.degeneracy_order), result.degeneracy};
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_DEGENERACY_ORDERING_HPP_INCLUDED
