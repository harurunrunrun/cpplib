#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_PERSISTENT_BITASSIGN_RANGESUM_RANGEFLIP_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_PERSISTENT_BITASSIGN_RANGESUM_RANGEFLIP_HPP_INCLUDED

#include "../detail/persistent_bit_range_tree.hpp"

template<long long MAX_SIZE = -1, int MAX_NODES = -1, int MAX_VERSIONS = -1>
struct PersistentBitAssignRangeSumRangeFlip:
    bit_range_tree_detail::PersistentBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS>{
    using Base = bit_range_tree_detail::PersistentBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS>;
    using Base::Base;
};

template<long long MAX_SIZE = -1, int MAX_NODES = -1, int MAX_VERSIONS = -1>
using PersistentDynamicBitAssignRangeSumRangeFlip =
    PersistentBitAssignRangeSumRangeFlip<MAX_SIZE, MAX_NODES, MAX_VERSIONS>;

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_PERSISTENT_BITASSIGN_RANGESUM_RANGEFLIP_HPP_INCLUDED
