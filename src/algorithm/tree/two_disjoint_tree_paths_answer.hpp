#pragma once

#include <utility>
#include <vector>

#include "tree_edge_split_diameters.hpp"

template<int MAX_SIZE>
TreeTwoDisjointPathsResult two_disjoint_tree_paths_answer(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    return maximum_product_of_two_vertex_disjoint_tree_paths<MAX_SIZE>(
        vertex_count,
        edges
    );
}
