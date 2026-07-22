#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_RANGE_ADD_RANGE_SUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_RANGE_ADD_RANGE_SUM_HPP_INCLUDED

#include <memory>

#include "dynamic_lazysegtree.hpp"
#include "../types/common_monoids.hpp"

template<class T, int MAX_SIZE>
class RangeAddRangeSum{
    static constexpr AddSumMonoidAct<T> action_{};
    std::unique_ptr<DynamicLazySegtree<action_, MAX_SIZE>> tree_;

public:
    explicit RangeAddRangeSum(int size)
        : tree_(std::make_unique<
            DynamicLazySegtree<action_, MAX_SIZE>
        >(size)){}

    int size() const{ return tree_->size(); }
    void add(int left, int right, T value){ tree_->apply(left, right, value); }
    T sum(int left, int right) const{ return tree_->prod(left, right); }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_RANGE_ADD_RANGE_SUM_HPP_INCLUDED
