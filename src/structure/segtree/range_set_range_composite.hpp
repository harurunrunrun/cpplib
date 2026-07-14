#pragma once

#include <memory>
#include <vector>

#include "../types/common_monoids.hpp"
#include "lazysegtree.hpp"

template<class T, int MAX_SIZE>
class RangeSetRangeComposite{
public:
    using affine_type = MonoidAffine<T>;

private:
    using aggregate_type = AffineCompositionAggregate<T>;
    using action_type = MonoidAssignment<affine_type>;
    using Tree = LazySegtree<
        AssignAffineCompositionMonoidAct<T>{},
        MAX_SIZE
    >;
    std::unique_ptr<Tree> tree_;
    int size_;

    static std::vector<aggregate_type> make_aggregates(
        const std::vector<affine_type>& functions
    ){
        std::vector<aggregate_type> aggregates;
        aggregates.reserve(functions.size());
        for(const affine_type& function: functions){
            aggregates.push_back({function, 1});
        }
        return aggregates;
    }

public:
    explicit RangeSetRangeComposite(
        const std::vector<affine_type>& functions
    ):
        tree_(std::make_unique<Tree>(make_aggregates(functions))),
        size_(static_cast<int>(functions.size())){}

    int size() const{
        return size_;
    }

    void assign(int left, int right, affine_type function){
        tree_->apply(left, right, action_type{true, function});
    }

    void assign(int left, int right, T multiplier, T addend){
        assign(left, right, affine_type{multiplier, addend});
    }

    T evaluate(int left, int right, T value){
        const affine_type function = tree_->prod(left, right).affine;
        return function.multiplier * value + function.addend;
    }
};
