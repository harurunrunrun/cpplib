#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

#include "reversible_splay_tree.hpp"
#include "../types/power_moment_monoid.hpp"

template<class T, std::size_t MAX_POWER, int MAX_SIZE>
class DynamicPowerMomentSequence{
    static constexpr PowerMomentMonoid<T, MAX_POWER> monoid_{};
    using Aggregate = PowerMomentAggregate<T, MAX_POWER>;

    std::unique_ptr<ReversibleSplayTree<monoid_, MAX_SIZE>> sequence_;

    static std::vector<Aggregate> make_aggregates(const std::vector<T>& values){
        std::vector<Aggregate> aggregates;
        aggregates.reserve(values.size());
        for(const T& value: values){
            aggregates.push_back(power_moment_singleton<T, MAX_POWER>(value));
        }
        return aggregates;
    }

    static Aggregate singleton(const T& value){
        return power_moment_singleton<T, MAX_POWER>(value);
    }

public:
    explicit DynamicPowerMomentSequence(const std::vector<T>& values)
        : sequence_(std::make_unique<ReversibleSplayTree<monoid_, MAX_SIZE>>(
              make_aggregates(values)
          )){}

    int size() const{ return sequence_->size(); }
    bool empty() const{ return sequence_->empty(); }

    void insert(int position, const T& value){
        sequence_->insert(position, singleton(value));
    }

    void erase(int position){ sequence_->erase(position); }

    void set(int position, const T& value){
        sequence_->set(position, singleton(value));
    }

    T power_moment(int left, int right, std::size_t power){
        if(power > MAX_POWER)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: power exceeds MAX_POWER "
                "(DynamicPowerMomentSequence::power_moment)."
            );
        }
        return sequence_->prod(left, right).moment[power];
    }
};
