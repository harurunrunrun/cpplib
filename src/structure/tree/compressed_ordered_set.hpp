#ifndef CPPLIB_SRC_STRUCTURE_TREE_COMPRESSED_ORDERED_SET_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_COMPRESSED_ORDERED_SET_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>
#include <utility>

#include "integer_set.hpp"

template<class Value, int MAX_UNIQUE>
class CompressedOrderedSet{
    static_assert(MAX_UNIQUE > 0);

    std::vector<Value> coordinates_;
    Int_Set<unsigned int, static_cast<unsigned int>(MAX_UNIQUE)> set_;

    unsigned int registered_index(const Value& value) const{
        const auto iterator = std::lower_bound(
            coordinates_.begin(), coordinates_.end(), value
        );
        if(iterator == coordinates_.end() || *iterator != value)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: unregistered value (CompressedOrderedSet)."
            );
        }
        return static_cast<unsigned int>(iterator - coordinates_.begin());
    }

    std::optional<Value> decode(std::optional<unsigned int> index) const{
        if(!index) return std::nullopt;
        return coordinates_[static_cast<std::size_t>(*index)];
    }

public:
    explicit CompressedOrderedSet(
        std::vector<Value> universe,
        const std::vector<Value>& initial = {}
    ): coordinates_(std::move(universe)){
        coordinates_.insert(coordinates_.end(), initial.begin(), initial.end());
        std::sort(coordinates_.begin(), coordinates_.end());
        coordinates_.erase(
            std::unique(coordinates_.begin(), coordinates_.end()),
            coordinates_.end()
        );
        if(coordinates_.size() > static_cast<std::size_t>(MAX_UNIQUE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity exceeded (CompressedOrderedSet)."
            );
        }
        for(const Value& value: initial) insert(value);
    }

    int size(){
        return static_cast<int>(set_.range_sum(
            0u, static_cast<unsigned int>(coordinates_.size())
        ));
    }

    bool empty(){ return set_.empty(); }

    void insert(const Value& value){ set_.insert(registered_index(value)); }
    void erase(const Value& value){ set_.erase(registered_index(value)); }
    bool contains(const Value& value){
        const auto iterator = std::lower_bound(
            coordinates_.begin(), coordinates_.end(), value
        );
        if(iterator == coordinates_.end() || *iterator != value) return false;
        return set_.contain(static_cast<unsigned int>(iterator - coordinates_.begin()));
    }

    std::optional<Value> kth_smallest(int index){
        if(index < 0) return std::nullopt;
        return decode(set_.kth_ge(0u, static_cast<unsigned int>(index)));
    }

    std::optional<Value> kth_smallest_one_based(int rank){
        if(rank <= 0) return std::nullopt;
        return kth_smallest(rank - 1);
    }

    int order_of_key(const Value& value){
        const auto index = static_cast<unsigned int>(
            std::lower_bound(coordinates_.begin(), coordinates_.end(), value) -
            coordinates_.begin()
        );
        return static_cast<int>(set_.range_sum(0u, index));
    }

    int count_less_equal(const Value& value){
        const auto index = static_cast<unsigned int>(
            std::upper_bound(coordinates_.begin(), coordinates_.end(), value) -
            coordinates_.begin()
        );
        return static_cast<int>(set_.range_sum(0u, index));
    }

    std::optional<Value> predecessor_or_equal(const Value& value){
        const auto iterator = std::upper_bound(
            coordinates_.begin(), coordinates_.end(), value
        );
        if(iterator == coordinates_.begin()) return std::nullopt;
        const auto index = static_cast<unsigned int>(
            iterator - coordinates_.begin() - 1
        );
        return decode(set_.most(index));
    }

    std::optional<Value> successor_or_equal(const Value& value){
        const auto iterator = std::lower_bound(
            coordinates_.begin(), coordinates_.end(), value
        );
        if(iterator == coordinates_.end()) return std::nullopt;
        const auto index = static_cast<unsigned int>(iterator - coordinates_.begin());
        return decode(set_.least(index));
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TREE_COMPRESSED_ORDERED_SET_HPP_INCLUDED
