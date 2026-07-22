#ifndef CPPLIB_SRC_ALGORITHM_TREE_MAXIMUM_PRODUCT_OF_TWO_VERTEX_DISJOINT_TREE_PATHS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_MAXIMUM_PRODUCT_OF_TWO_VERTEX_DISJOINT_TREE_PATHS_HPP_INCLUDED

#include "tree_edge_split_diameters.hpp"

#include <cstddef>
#include <utility>
#include <vector>

struct TreeTwoDisjointPathsResult{
    long long product;
    int cut_edge_index;
    int cut_u;
    int cut_v;
    int first_diameter;
    int second_diameter;

    friend constexpr bool operator==(
        const TreeTwoDisjointPathsResult&,
        const TreeTwoDisjointPathsResult&
    ) = default;
};

template<int MAX_SIZE>
TreeTwoDisjointPathsResult maximum_product_of_two_vertex_disjoint_tree_paths(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    const std::vector<TreeEdgeSplitDiameter> split =
        tree_edge_split_diameters<MAX_SIZE>(vertex_count, edges);
    TreeTwoDisjointPathsResult result{0, -1, -1, -1, 0, 0};
    for(int edge = 0; edge < static_cast<int>(split.size()); ++edge){
        const TreeEdgeSplitDiameter current =
            split[static_cast<std::size_t>(edge)];
        const long long product =
            static_cast<long long>(current.first_diameter)
            * current.second_diameter;
        if(result.cut_edge_index != -1 && product <= result.product) continue;
        result = {
            product,
            edge,
            edges[static_cast<std::size_t>(edge)].first,
            edges[static_cast<std::size_t>(edge)].second,
            current.first_diameter,
            current.second_diameter
        };
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_MAXIMUM_PRODUCT_OF_TWO_VERTEX_DISJOINT_TREE_PATHS_HPP_INCLUDED
