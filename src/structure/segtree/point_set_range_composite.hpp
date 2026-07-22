#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_POINT_SET_RANGE_COMPOSITE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_POINT_SET_RANGE_COMPOSITE_HPP_INCLUDED

#include <memory>
#include <vector>

#include "../types/common_monoids.hpp"
#include "segtree.hpp"

template<class T, int MAX_SIZE>
class PointSetRangeComposite{
public:
    using affine_type = MonoidAffine<T>;

private:
    using Tree = Segtree<AffineCompositionMonoid<T>{}, MAX_SIZE>;
    std::unique_ptr<Tree> tree_;
    int size_;

public:
    explicit PointSetRangeComposite(int size):
        tree_(std::make_unique<Tree>(size)), size_(size){}

    explicit PointSetRangeComposite(const std::vector<affine_type>& functions):
        tree_(std::make_unique<Tree>(functions)),
        size_(static_cast<int>(functions.size())){}

    int size() const{
        return size_;
    }

    void set(int index, affine_type function){
        tree_->set(index, function);
    }

    void set(int index, T multiplier, T addend){
        set(index, affine_type{multiplier, addend});
    }

    T evaluate(int left, int right, T value) const{
        const affine_type function = tree_->prod(left, right);
        return function.multiplier * value + function.addend;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_POINT_SET_RANGE_COMPOSITE_HPP_INCLUDED
