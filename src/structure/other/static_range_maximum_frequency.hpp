#pragma once

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <vector>

#include "sparse_table.hpp"
#include "../types/common_monoids.hpp"

template<class T, int MAX_SIZE>
class StaticRangeMaximumFrequency{
    static constexpr MaxMonoid<int> maximum_{};
    std::vector<int> group_;
    std::vector<int> begin_;
    std::vector<int> end_;
    std::unique_ptr<SparseTable<maximum_, MAX_SIZE>> table_;

public:
    explicit StaticRangeMaximumFrequency(const std::vector<T>& values){
        if(!std::is_sorted(values.begin(), values.end()))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: values are not sorted "
                "(StaticRangeMaximumFrequency)."
            );
        }
        group_.resize(values.size());
        std::vector<int> frequencies;
        for(int left = 0; left < static_cast<int>(values.size());){
            int right = left + 1;
            while(right < static_cast<int>(values.size())
                && values[static_cast<std::size_t>(right)]
                    == values[static_cast<std::size_t>(left)]) ++right;
            const int group = static_cast<int>(frequencies.size());
            begin_.push_back(left);
            end_.push_back(right);
            frequencies.push_back(right - left);
            std::fill(group_.begin() + left, group_.begin() + right, group);
            left = right;
        }
        table_ = std::make_unique<SparseTable<maximum_, MAX_SIZE>>(frequencies);
    }

    int size() const{ return static_cast<int>(group_.size()); }

    int maximum_frequency(int left, int right) const{
        if(left < 0 || right <= left || size() < right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid range "
                "(StaticRangeMaximumFrequency)."
            );
        }
        const int left_group = group_[static_cast<std::size_t>(left)];
        const int right_group = group_[static_cast<std::size_t>(right - 1)];
        if(left_group == right_group) return right - left;
        return std::max({
            end_[static_cast<std::size_t>(left_group)] - left,
            table_->prod(left_group + 1, right_group),
            right - begin_[static_cast<std::size_t>(right_group)]
        });
    }
};
