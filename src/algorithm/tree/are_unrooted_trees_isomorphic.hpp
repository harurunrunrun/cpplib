#pragma once

#include <vector>

#include "tree_isomorphism.hpp"

inline bool are_unrooted_trees_isomorphic(
    const std::vector<std::vector<int>>& first,
    const std::vector<std::vector<int>>& second
){
    return tree_isomorphic(first, second);
}
