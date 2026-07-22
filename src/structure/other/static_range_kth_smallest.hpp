#ifndef CPPLIB_SRC_STRUCTURE_OTHER_STATIC_RANGE_KTH_SMALLEST_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_STATIC_RANGE_KTH_SMALLEST_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "../../algorithm/tree/query/tree_path_order_statistics.hpp"

template<class Value, int MAX_SIZE>
class StaticRangeKthSmallest{
    static_assert(MAX_SIZE > 0);

    int size_;
    TreePathOrderStatistics<Value, MAX_SIZE> statistics_;

    static int checked_size(std::size_t size){
        if(static_cast<std::size_t>(MAX_SIZE) < size)[[unlikely]]{
            throw std::length_error(
                "StaticRangeKthSmallest size capacity exceeded"
            );
        }
        return static_cast<int>(size);
    }

public:
    explicit StaticRangeKthSmallest(const std::vector<Value>& values):
        size_(checked_size(values.size())), statistics_(size_){
        for(int index = 1; index < size_; ++index){
            statistics_.add_edge(index - 1, index);
        }
        statistics_.build(values);
    }

    StaticRangeKthSmallest(const StaticRangeKthSmallest&) = delete;
    StaticRangeKthSmallest& operator=(const StaticRangeKthSmallest&) = delete;
    StaticRangeKthSmallest(StaticRangeKthSmallest&&) = default;
    StaticRangeKthSmallest& operator=(StaticRangeKthSmallest&&) = default;

    int size() const noexcept{ return size_; }
    bool empty() const noexcept{ return size_ == 0; }

    Value kth_smallest(int left, int right, int order) const{
        if(left < 0 || right < left || size_ < right)[[unlikely]]{
            throw std::out_of_range(
                "StaticRangeKthSmallest query range is out of range"
            );
        }
        if(order < 0 || right - left <= order)[[unlikely]]{
            throw std::out_of_range(
                "StaticRangeKthSmallest order is out of range"
            );
        }
        return statistics_.kth_smallest(left, right - 1, order);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_STATIC_RANGE_KTH_SMALLEST_HPP_INCLUDED
