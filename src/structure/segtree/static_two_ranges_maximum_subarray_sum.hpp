#pragma once

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>

#include "segtree.hpp"
#include "../types/common_monoids.hpp"

template<class T, int MAX_SIZE>
class StaticTwoRangesMaximumSubarraySum{
    static constexpr NonEmptyMaxSubarrayMonoid<T> monoid_{};
    std::unique_ptr<Segtree<monoid_, MAX_SIZE>> tree_;
    int size_ = 0;

public:
    explicit StaticTwoRangesMaximumSubarraySum(const std::vector<T>& values){
        std::vector<NonEmptyMaxSubarrayAggregate<T>> initial;
        size_ = static_cast<int>(values.size());
        initial.reserve(values.size());
        for(const T& value: values){
            initial.push_back(non_empty_max_subarray_singleton(value));
        }
        tree_ = std::make_unique<Segtree<monoid_, MAX_SIZE>>(initial);
    }

    int size() const{ return size_; }

    T maximum_sum(
        int first_left,
        int first_right,
        int second_left,
        int second_right
    ) const{
        if(first_left < 0 || first_left >= first_right
            || first_right > second_right || second_left < first_left
            || second_left >= second_right || size() < second_right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid ranges "
                "(StaticTwoRangesMaximumSubarraySum)."
            );
        }
        if(first_right <= second_left){
            return tree_->prod(first_left, first_right).suffix
                + tree_->prod(first_right, second_left).sum
                + tree_->prod(second_left, second_right).prefix;
        }
        const auto overlap = tree_->prod(second_left, first_right);
        T answer = overlap.best;
        if(first_left < second_left){
            answer = std::max(
                answer,
                tree_->prod(first_left, second_left).suffix
                    + tree_->prod(second_left, second_right).prefix
            );
        }
        if(first_right < second_right){
            answer = std::max(
                answer,
                overlap.suffix
                    + tree_->prod(first_right, second_right).prefix
            );
        }
        return answer;
    }
};
