#pragma once

#include "detail/partially_persistent_bit_range_tree.hpp"

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS, int MAX_CHANGES>
struct PartiallyPersistentBitOverwriteRangeSum:
    bit_range_tree_detail::PartiallyPersistentBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>{
    using Base = bit_range_tree_detail::PartiallyPersistentBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>;
    using Base::Base;

    int set(int version, long long left, long long right){
        return this->set_one(version, left, right);
    }
    int set(long long left, long long right){ return this->set_one(left, right); }
};

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS, int MAX_CHANGES>
using PartiallyPersistentDynamicBitOverwriteRangeSum =
    PartiallyPersistentBitOverwriteRangeSum<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>;
