#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_POINT_SET_RANGE_COMPOSITE_LARGE_ARRAY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_POINT_SET_RANGE_COMPOSITE_LARGE_ARRAY_HPP_INCLUDED

#include <stdexcept>

#include "../types/common_monoids.hpp"
#include "dynamic_segtree.hpp"

template<class T, long long MAX_SIZE>
class PointSetRangeCompositeLargeArray{
public:
    using affine_type = MonoidAffine<T>;

private:
    DynamicSegtree<AffineCompositionMonoid<T>{}, MAX_SIZE> tree_;
    long long size_;

    void check_index(long long index) const{
        if(index < 0 || size_ <= index){
            throw std::runtime_error(
                "library assertion fault: range violation (index)."
            );
        }
    }

    void check_range(long long left, long long right) const{
        if(left < 0 || left > right || size_ < right){
            throw std::runtime_error(
                "library assertion fault: range violation (range)."
            );
        }
    }

public:
    explicit PointSetRangeCompositeLargeArray(long long size): size_(size){
        if(size < 0 || MAX_SIZE < size){
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
    }

    long long size() const{
        return size_;
    }

    void set(long long index, affine_type function){
        check_index(index);
        tree_.set(index, function);
    }

    void set(long long index, T multiplier, T addend){
        set(index, affine_type{multiplier, addend});
    }

    T evaluate(long long left, long long right, T value) const{
        check_range(left, right);
        const affine_type function = tree_.prod(left, right);
        return function.multiplier * value + function.addend;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_POINT_SET_RANGE_COMPOSITE_LARGE_ARRAY_HPP_INCLUDED
