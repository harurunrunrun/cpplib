#pragma once

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
