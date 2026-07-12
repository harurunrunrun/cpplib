#pragma once

#include "detail/persistent_bit_range_tree.hpp"

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS>
struct PersistentBitAssignRangeSumRangeFlip:
    bit_range_tree_detail::PersistentBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS>{
    using Base = bit_range_tree_detail::PersistentBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS>;
    using Base::Base;
};

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS>
using PersistentDynamicBitAssignRangeSumRangeFlip =
    PersistentBitAssignRangeSumRangeFlip<MAX_SIZE, MAX_NODES, MAX_VERSIONS>;
