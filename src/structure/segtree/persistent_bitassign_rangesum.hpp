#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_BITASSIGN_RANGESUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_BITASSIGN_RANGESUM_HPP_INCLUDED

#include "detail/persistent_bit_range_tree.hpp"

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS>
struct PersistentBitAssignRangeSum:
    bit_range_tree_detail::PersistentBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS>{
    using Base = bit_range_tree_detail::PersistentBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS>;
    using Base::Base;
};

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS>
using PersistentDynamicBitAssignRangeSum =
    PersistentBitAssignRangeSum<MAX_SIZE, MAX_NODES, MAX_VERSIONS>;

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_BITASSIGN_RANGESUM_HPP_INCLUDED
