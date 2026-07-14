#pragma once

#include <memory>
#include <vector>

#include "sparse_table.hpp"
#include "../types/common_monoids.hpp"

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
