#ifndef CPPLIB_SRC_STRUCTURE_OTHER_THRESHOLD_UPDATED_INDEXED_MULTISET_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_THRESHOLD_UPDATED_INDEXED_MULTISET_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../segtree/lazysegtree.hpp"
#include "../types/common_monoids.hpp"

template<int MAX_SIZE = 100000>
class ThresholdUpdatedIndexedMultiset{
    static_assert(MAX_SIZE >= 0);
    using Wide = __int128_t;

    static constexpr AddMaxMonoidAct<Wide> action_{};
    std::unique_ptr<LazySegtree<action_, MAX_SIZE>> tree_;
    std::vector<int> original_at_position_;
    std::vector<int> position_of_original_;

    int first_at_least(long long threshold) const{
        return tree_->max_right(
            0,
            [](Wide maximum, Wide limit){ return maximum < limit; },
            static_cast<Wide>(threshold)
        );
    }

public:
    explicit ThresholdUpdatedIndexedMultiset(
        const std::vector<long long>& values
    ){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE) ||
           values.size() >
               static_cast<std::size_t>(std::numeric_limits<int>::max()))[[unlikely]]{
            throw std::length_error(
                "ThresholdUpdatedIndexedMultiset size exceeds capacity"
            );
        }
        const int size = static_cast<int>(values.size());
        std::vector<std::pair<long long, int>> order;
        order.reserve(values.size());
        for(int index = 0; index < size; ++index){
            order.emplace_back(values[static_cast<std::size_t>(index)], index);
        }
        std::sort(order.begin(), order.end());
        std::vector<Wide> sorted(values.begin(), values.end());
        original_at_position_.resize(values.size());
        position_of_original_.resize(values.size());
        for(int position = 0; position < size; ++position){
            const auto [value, original] = order[static_cast<std::size_t>(position)];
            sorted[static_cast<std::size_t>(position)] = value;
            original_at_position_[static_cast<std::size_t>(position)] = original;
            position_of_original_[static_cast<std::size_t>(original)] = position;
        }
        tree_ = std::make_unique<LazySegtree<action_, MAX_SIZE>>(sorted);
    }

    int size() const{ return static_cast<int>(position_of_original_.size()); }

    void increment(int original_index){
        if(original_index < 0 || size() <= original_index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: index out of range "
                "(ThresholdUpdatedIndexedMultiset)."
            );
        }
        const int position = position_of_original_[static_cast<std::size_t>(original_index)];
        const Wide value = tree_->get(position);
        if(value == static_cast<Wide>(
            std::numeric_limits<long long>::max()
        ))[[unlikely]]{
            throw std::overflow_error(
                "ThresholdUpdatedIndexedMultiset increment overflow"
            );
        }
        const int last_equal = first_at_least(static_cast<long long>(value + 1)) - 1;
        const int displaced = original_at_position_[static_cast<std::size_t>(last_equal)];
        std::swap(
            original_at_position_[static_cast<std::size_t>(position)],
            original_at_position_[static_cast<std::size_t>(last_equal)]
        );
        position_of_original_[static_cast<std::size_t>(original_index)] = last_equal;
        position_of_original_[static_cast<std::size_t>(displaced)] = position;
        tree_->apply(last_equal, static_cast<Wide>(1));
    }

    int count_at_least(long long threshold) const{
        return size() - first_at_least(threshold);
    }

    void decrement_at_least(long long threshold){
        const int first = first_at_least(threshold);
        if(first == size()) return;
        if(tree_->get(first) == static_cast<Wide>(
            std::numeric_limits<long long>::min()
        ))[[unlikely]]{
            throw std::overflow_error(
                "ThresholdUpdatedIndexedMultiset decrement overflow"
            );
        }
        tree_->apply(first, size(), static_cast<Wide>(-1));
    }

    long long get(int original_index) const{
        if(original_index < 0 || size() <= original_index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: index out of range "
                "(ThresholdUpdatedIndexedMultiset)."
            );
        }
        return static_cast<long long>(tree_->get(
            position_of_original_[static_cast<std::size_t>(original_index)]
        ));
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_THRESHOLD_UPDATED_INDEXED_MULTISET_HPP_INCLUDED
