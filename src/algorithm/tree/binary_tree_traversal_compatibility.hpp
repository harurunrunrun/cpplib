#pragma once

#include <vector>

#include "binary_tree_traversal_reconstruction.hpp"

template<int MAX_SIZE>
bool binary_tree_traversal_compatibility(
    const std::vector<int>& preorder,
    const std::vector<int>& postorder,
    const std::vector<int>& inorder
){
    return binary_tree_traversals_compatible<MAX_SIZE>(preorder, inorder, postorder);
}
