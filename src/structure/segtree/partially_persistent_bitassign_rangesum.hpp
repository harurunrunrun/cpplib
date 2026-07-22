#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_BITASSIGN_RANGESUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_BITASSIGN_RANGESUM_HPP_INCLUDED

#include "detail/partially_persistent_bit_range_tree.hpp"

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS, int MAX_CHANGES>
struct PartiallyPersistentBitAssignRangeSum:
    bit_range_tree_detail::PartiallyPersistentBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>{
    using Base = bit_range_tree_detail::PartiallyPersistentBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>;
    using Base::Base;
};

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS, int MAX_CHANGES>
using PartiallyPersistentDynamicBitAssignRangeSum =
    PartiallyPersistentBitAssignRangeSum<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>;

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_BITASSIGN_RANGESUM_HPP_INCLUDED
