#pragma once

#include "detail/persistent_bit_range_tree.hpp"

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS>
struct PersistentBitOverwriteRangeSum:
    bit_range_tree_detail::PersistentBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS>{
    using Base = bit_range_tree_detail::PersistentBitRangeTree<MAX_SIZE, MAX_NODES, MAX_VERSIONS>;
    using Base::Base;

    int set(int version, long long left, long long right){
        return this->set_one(version, left, right);
    }
};

template<long long MAX_SIZE, int MAX_NODES, int MAX_VERSIONS>
using PersistentDynamicBitOverwriteRangeSum =
    PersistentBitOverwriteRangeSum<MAX_SIZE, MAX_NODES, MAX_VERSIONS>;
