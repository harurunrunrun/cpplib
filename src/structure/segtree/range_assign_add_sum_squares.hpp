#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_RANGE_ASSIGN_ADD_SUM_SQUARES_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_RANGE_ASSIGN_ADD_SUM_SQUARES_HPP_INCLUDED

#include <memory>
#include <vector>

#include "lazysegtree.hpp"
#include "../types/sum_squares_monoid.hpp"

template<class T, int MAX_SIZE>
class RangeAssignAddSumSquares{
    static constexpr AffineSumSquaresMonoidAct<T> action_{};
    using Aggregate = SumSquaresAggregate<T>;

    int size_;
    std::unique_ptr<LazySegtree<action_, MAX_SIZE>> tree_;

    static std::vector<Aggregate> make_aggregates(const std::vector<T>& values){
        std::vector<Aggregate> aggregates;
        aggregates.reserve(values.size());
        for(const T& value: values){
            aggregates.push_back(sum_squares_singleton(value));
        }
        return aggregates;
    }

public:
    explicit RangeAssignAddSumSquares(const std::vector<T>& values)
        : size_(static_cast<int>(values.size())),
          tree_(std::make_unique<LazySegtree<action_, MAX_SIZE>>(
              make_aggregates(values)
          )){}

    int size() const{ return size_; }

    void assign(int left, int right, const T& value){
        tree_->apply(left, right, sum_squares_assign(value));
    }

    void add(int left, int right, const T& value){
        tree_->apply(left, right, sum_squares_add(value));
    }

    T square_sum(int left, int right){
        return tree_->prod(left, right).square_sum;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_RANGE_ASSIGN_ADD_SUM_SQUARES_HPP_INCLUDED
