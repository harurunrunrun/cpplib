#pragma once

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../segtree/lazysegtree.hpp"
#include "../types/common_monoids.hpp"

template<int MAX_SIZE = 100000>
class SortedQueueLengths{
    static constexpr AddMaxMonoidAct<long long> action_{};
    std::unique_ptr<LazySegtree<action_, MAX_SIZE>> tree_;
    std::vector<int> original_at_position_;
    std::vector<int> position_of_original_;

    int first_at_least(long long threshold) const{
        return tree_->max_right(
            0,
            [](long long maximum, long long limit){ return maximum < limit; },
            threshold
        );
    }

public:
    explicit SortedQueueLengths(const std::vector<long long>& lengths){
        const int size = static_cast<int>(lengths.size());
        std::vector<std::pair<long long, int>> order;
        order.reserve(lengths.size());
        for(int index = 0; index < size; ++index){
            order.emplace_back(lengths[static_cast<std::size_t>(index)], index);
        }
        std::sort(order.begin(), order.end());
        std::vector<long long> sorted(lengths.size());
        original_at_position_.resize(lengths.size());
        position_of_original_.resize(lengths.size());
        for(int position = 0; position < size; ++position){
            const auto [length, original] = order[static_cast<std::size_t>(position)];
            sorted[static_cast<std::size_t>(position)] = length;
            original_at_position_[static_cast<std::size_t>(position)] = original;
            position_of_original_[static_cast<std::size_t>(original)] = position;
        }
        tree_ = std::make_unique<LazySegtree<action_, MAX_SIZE>>(sorted);
    }

    int size() const{ return static_cast<int>(position_of_original_.size()); }

    void increment(int original_index){
        if(original_index < 0 || size() <= original_index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: index out of range (SortedQueueLengths)."
            );
        }
        const int position = position_of_original_[static_cast<std::size_t>(original_index)];
        const long long length = tree_->get(position);
        const int last_equal = first_at_least(length + 1) - 1;
        const int displaced = original_at_position_[static_cast<std::size_t>(last_equal)];
        std::swap(
            original_at_position_[static_cast<std::size_t>(position)],
            original_at_position_[static_cast<std::size_t>(last_equal)]
        );
        position_of_original_[static_cast<std::size_t>(original_index)] = last_equal;
        position_of_original_[static_cast<std::size_t>(displaced)] = position;
        tree_->apply(last_equal, 1LL);
    }

    int count_at_least(long long threshold) const{
        return size() - first_at_least(threshold);
    }

    void decrement_at_least(long long threshold){
        tree_->apply(first_at_least(threshold), size(), -1LL);
    }

    long long get(int original_index) const{
        if(original_index < 0 || size() <= original_index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: index out of range (SortedQueueLengths)."
            );
        }
        return tree_->get(
            position_of_original_[static_cast<std::size_t>(original_index)]
        );
    }
};
