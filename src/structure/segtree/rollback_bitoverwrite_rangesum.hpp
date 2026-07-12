#pragma once

#include "detail/rollback_bit_range_tree.hpp"

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS, int MAX_CHANGES>
struct RollbackBitOverwriteRangeSum:
    bit_range_tree_detail::RollbackBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>{
    using Base = bit_range_tree_detail::RollbackBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>;
    using Base::Base;

    int set(long long left, long long right){ return this->set_one(left, right); }
};

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS, int MAX_CHANGES>
using RollbackDynamicBitOverwriteRangeSum =
    RollbackBitOverwriteRangeSum<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>;
