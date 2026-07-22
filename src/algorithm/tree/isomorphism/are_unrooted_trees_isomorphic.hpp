#ifndef CPPLIB_SRC_ALGORITHM_TREE_ISOMORPHISM_ARE_UNROOTED_TREES_ISOMORPHIC_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_ISOMORPHISM_ARE_UNROOTED_TREES_ISOMORPHIC_HPP_INCLUDED

#include <vector>

#include "tree_isomorphism.hpp"

inline bool are_unrooted_trees_isomorphic(
    const std::vector<std::vector<int>>& first,
    const std::vector<std::vector<int>>& second
){
    return tree_isomorphic(first, second);
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_ISOMORPHISM_ARE_UNROOTED_TREES_ISOMORPHIC_HPP_INCLUDED
