#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_ROLLBACK_BITASSIGN_RANGESUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_ROLLBACK_BITASSIGN_RANGESUM_HPP_INCLUDED

#include "../detail/rollback_bit_range_tree.hpp"

template<long long MAX_SIZE = -1, int MAX_NODES = -1, int MAX_VERSIONS = -1, int MAX_CHANGES = -1>
struct RollbackBitAssignRangeSum:
    bit_range_tree_detail::RollbackBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>{
    using Base = bit_range_tree_detail::RollbackBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>;
    using Base::Base;
};

template<long long MAX_SIZE = -1, int MAX_NODES = -1, int MAX_VERSIONS = -1, int MAX_CHANGES = -1>
using RollbackDynamicBitAssignRangeSum =
    RollbackBitAssignRangeSum<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>;

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_ROLLBACK_BITASSIGN_RANGESUM_HPP_INCLUDED
