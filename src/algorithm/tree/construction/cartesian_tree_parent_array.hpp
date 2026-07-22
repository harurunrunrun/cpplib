#ifndef CPPLIB_SRC_ALGORITHM_TREE_CONSTRUCTION_CARTESIAN_TREE_PARENT_ARRAY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_CONSTRUCTION_CARTESIAN_TREE_PARENT_ARRAY_HPP_INCLUDED

#include <functional>
#include <utility>
#include <vector>

#include "cartesian_tree.hpp"

template<class T, class Compare = std::less<T>>
std::vector<int> cartesian_tree_parent_array(
    const std::vector<T>& values,
    Compare compare = Compare()
){
    CartesianTreeResult tree = cartesian_tree(values, std::move(compare));
    if(tree.root != -1){
        tree.parent[static_cast<std::size_t>(tree.root)] = tree.root;
    }
    return std::move(tree.parent);
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_CONSTRUCTION_CARTESIAN_TREE_PARENT_ARRAY_HPP_INCLUDED
