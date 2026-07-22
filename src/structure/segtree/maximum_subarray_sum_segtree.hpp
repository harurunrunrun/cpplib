#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_MAXIMUM_SUBARRAY_SUM_SEGTREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_MAXIMUM_SUBARRAY_SUM_SEGTREE_HPP_INCLUDED

#include <memory>
#include <stdexcept>
#include <vector>

#include "segtree.hpp"
#include "../types/common_monoids.hpp"

template<class T, int MAX_SIZE>
class MaximumSubarraySumSegtree{
    static constexpr NonEmptyMaxSubarrayMonoid<T> monoid_{};
    using Aggregate = NonEmptyMaxSubarrayAggregate<T>;

    int size_;
    std::unique_ptr<Segtree<monoid_, MAX_SIZE>> tree_;

    static std::vector<Aggregate> make_aggregates(const std::vector<T>& values){
        std::vector<Aggregate> aggregates;
        aggregates.reserve(values.size());
        for(const T& value: values){
            aggregates.push_back(non_empty_max_subarray_singleton(value));
        }
        return aggregates;
    }

public:
    explicit MaximumSubarraySumSegtree(const std::vector<T>& values)
        : size_(static_cast<int>(values.size())),
          tree_(std::make_unique<Segtree<monoid_, MAX_SIZE>>(
              make_aggregates(values)
          )){}

    int size() const{ return size_; }

    void set(int position, const T& value){
        tree_->set(position, non_empty_max_subarray_singleton(value));
    }

    T maximum_sum(int left, int right) const{
        if(right <= left)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range must be non-empty "
                "(MaximumSubarraySumSegtree::maximum_sum)."
            );
        }
        return tree_->prod(left, right).best;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_MAXIMUM_SUBARRAY_SUM_SEGTREE_HPP_INCLUDED
