#pragma once

#include <memory>
#include <vector>

#include "../types/common_monoids.hpp"
#include "dualsegtree.hpp"

template<class T, int MAX_SIZE>
class RangeAffinePointGet{
public:
    using affine_type = MonoidAffine<T>;

private:
    using aggregate_type = SumLengthAggregate<T>;
    using Tree = DualSegtree<AffineSumAggregateMonoidAct<T>{}, MAX_SIZE>;
    std::unique_ptr<Tree> tree_;
    int size_;

    static std::vector<aggregate_type> make_aggregates(
        const std::vector<T>& values
    ){
        std::vector<aggregate_type> aggregates;
        aggregates.reserve(values.size());
        for(const T& value: values) aggregates.push_back({value, 1});
        return aggregates;
    }

public:
    explicit RangeAffinePointGet(const std::vector<T>& values):
        tree_(std::make_unique<Tree>(make_aggregates(values))),
        size_(static_cast<int>(values.size())){}

    int size() const{
        return size_;
    }

    void apply(int left, int right, affine_type function){
        tree_->apply(left, right, function);
    }

    void apply(int left, int right, T multiplier, T addend){
        apply(left, right, affine_type{multiplier, addend});
    }

    T get(int index){
        return tree_->get(index).sum;
    }
};
