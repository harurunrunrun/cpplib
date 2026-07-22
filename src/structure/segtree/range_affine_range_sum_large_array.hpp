#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_RANGE_AFFINE_RANGE_SUM_LARGE_ARRAY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_RANGE_AFFINE_RANGE_SUM_LARGE_ARRAY_HPP_INCLUDED

#include "../types/common_monoids.hpp"
#include "dynamic_lazysegtree.hpp"

template<class T, long long MAX_SIZE>
class RangeAffineRangeSumLargeArray{
public:
    using affine_type = MonoidAffine<T>;

private:
    DynamicLazySegtree<AffineSumMonoidAct<T>{}, MAX_SIZE> tree_;
    long long size_;

public:
    explicit RangeAffineRangeSumLargeArray(long long size):
        tree_(size), size_(size){}

    long long size() const{
        return size_;
    }

    void apply(long long left, long long right, affine_type function){
        tree_.apply(left, right, function);
    }

    void apply(
        long long left,
        long long right,
        T multiplier,
        T addend
    ){
        apply(left, right, affine_type{multiplier, addend});
    }

    T range_sum(long long left, long long right) const{
        return tree_.prod(left, right);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_RANGE_AFFINE_RANGE_SUM_LARGE_ARRAY_HPP_INCLUDED
