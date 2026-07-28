#ifndef CPPLIB_SRC_STRUCTURE_BBST_MULTIWAY_B_PLUS_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_MULTIWAY_B_PLUS_TREE_HPP_INCLUDED

#include <cstddef>
#include <functional>

#include "detail/standard_b_plus_tree.hpp"

template<class Key, class Value, std::size_t FANOUT = 32,
         class Compare = std::less<Key>>
using BPlusTree = StandardBPlusTree<Key, Value, FANOUT, Compare>;

#endif  // CPPLIB_SRC_STRUCTURE_BBST_MULTIWAY_B_PLUS_TREE_HPP_INCLUDED
