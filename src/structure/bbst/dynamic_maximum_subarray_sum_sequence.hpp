#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#include "reversible_splay_tree.hpp"
#include "../types/common_monoids.hpp"

template<class T, int MAX_SIZE>
class DynamicMaximumSubarraySumSequence{
    static constexpr NonEmptyMaxSubarrayMonoid<T> monoid_{};
    using Aggregate = NonEmptyMaxSubarrayAggregate<T>;

    std::unique_ptr<ReversibleSplayTree<monoid_, MAX_SIZE>> sequence_;

    static std::vector<Aggregate> make_aggregates(const std::vector<T>& values){
        std::vector<Aggregate> aggregates;
        aggregates.reserve(values.size());
        for(const T& value: values){
            aggregates.push_back(non_empty_max_subarray_singleton(value));
        }
        return aggregates;
    }

public:
    explicit DynamicMaximumSubarraySumSequence(const std::vector<T>& values)
        : sequence_(std::make_unique<ReversibleSplayTree<monoid_, MAX_SIZE>>(
              make_aggregates(values)
          )){}

    int size() const{ return sequence_->size(); }
    bool empty() const{ return sequence_->empty(); }

    void insert(int position, const T& value){
        sequence_->insert(position, non_empty_max_subarray_singleton(value));
    }

    void erase(int position){ sequence_->erase(position); }

    void set(int position, const T& value){
        sequence_->set(position, non_empty_max_subarray_singleton(value));
    }

    T maximum_sum(int left, int right){
        if(right <= left)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range must be non-empty "
                "(DynamicMaximumSubarraySumSequence::maximum_sum)."
            );
        }
        return sequence_->prod(left, right).best;
    }
};
