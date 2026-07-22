#ifndef CPPLIB_SRC_STRUCTURE_DSU_RANGE_PARALLEL_COMPONENT_PRODUCT_SUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_DSU_RANGE_PARALLEL_COMPONENT_PRODUCT_SUM_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "range_parallel_unionfind.hpp"

template<int MAX_SIZE, class T>
class RangeParallelComponentProductSum{
    std::unique_ptr<RangeParallelUnionFind<MAX_SIZE>> dsu_;
    static int checked_size(std::size_t size){
        if(size > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
           size > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::length_error(
                "RangeParallelComponentProductSum size is out of range"
            );
        }
        return static_cast<int>(size);
    }

    std::vector<T> component_sum_;
    T product_sum_{};

public:
    explicit RangeParallelComponentProductSum(std::vector<T> vertex_weights):
        dsu_(std::make_unique<RangeParallelUnionFind<MAX_SIZE>>(
            checked_size(vertex_weights.size())
        )),
        component_sum_(std::move(vertex_weights)){}

    int size() const noexcept{ return dsu_->size(); }
    int groups() const noexcept{ return dsu_->groups(); }

    T product_sum() const{ return product_sum_; }

    T component_sum(int vertex){
        return component_sum_[static_cast<std::size_t>(dsu_->leader(vertex))];
    }

    bool same(int left, int right){
        return dsu_->same(left, right);
    }

    T merge_ranges(
        int first_left,
        int first_right,
        int second_left,
        int second_right
    ){
        dsu_->merge_ranges(
            first_left,
            first_right,
            second_left,
            second_right,
            [&](int root, int absorbed){
                product_sum_ +=
                    component_sum_[static_cast<std::size_t>(root)] *
                    component_sum_[static_cast<std::size_t>(absorbed)];
                component_sum_[static_cast<std::size_t>(root)] +=
                    component_sum_[static_cast<std::size_t>(absorbed)];
            }
        );
        return product_sum_;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_DSU_RANGE_PARALLEL_COMPONENT_PRODUCT_SUM_HPP_INCLUDED
