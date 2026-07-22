#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_BITASSIGN_RANGESUM_RANGEFLIP_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_BITASSIGN_RANGESUM_RANGEFLIP_HPP_INCLUDED

#include "detail/rollback_bit_range_tree.hpp"

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS, int MAX_CHANGES>
struct RollbackBitAssignRangeSumRangeFlip:
    bit_range_tree_detail::RollbackBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>{
    using Base = bit_range_tree_detail::RollbackBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>;
    using Base::Base;
};

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS, int MAX_CHANGES>
using RollbackDynamicBitAssignRangeSumRangeFlip =
    RollbackBitAssignRangeSumRangeFlip<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>;

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_ROLLBACK_BITASSIGN_RANGESUM_RANGEFLIP_HPP_INCLUDED
