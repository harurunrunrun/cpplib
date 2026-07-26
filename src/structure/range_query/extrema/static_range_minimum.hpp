#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_STATIC_RANGE_MINIMUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_STATIC_RANGE_MINIMUM_HPP_INCLUDED

#include <memory>
#include <vector>

#include "../aggregation/sparse_table.hpp"
#include "../../types/monoid/common_monoids.hpp"

template<class T, int MAX_SIZE>
class StaticRangeMinimum{
    static constexpr MinMonoid<T> minimum_{};
    std::unique_ptr<SparseTable<minimum_, MAX_SIZE>> table_;

public:
    explicit StaticRangeMinimum(const std::vector<T>& values)
        : table_(std::make_unique<
            SparseTable<minimum_, MAX_SIZE>
        >(values)){}

    int size() const{ return table_->size(); }
    T minimum(int left, int right) const{ return table_->prod(left, right); }
};

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_STATIC_RANGE_MINIMUM_HPP_INCLUDED
