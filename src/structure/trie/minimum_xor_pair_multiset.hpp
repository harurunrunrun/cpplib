#pragma once

#include <cstddef>
#include <iterator>
#include <set>
#include <stdexcept>
#include <type_traits>

template<class T>
class MinimumXorPairMultiset{
    static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>);

    std::multiset<T> values;
    std::multiset<T> adjacent_xors;

    void erase_xor(typename std::multiset<T>::const_iterator left,
                   typename std::multiset<T>::const_iterator right){
        const auto position = adjacent_xors.find(*left ^ *right);
        if(position == adjacent_xors.end())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: inconsistent state "
                "(MinimumXorPairMultiset)."
            );
        }
        adjacent_xors.erase(position);
    }

public:
    std::size_t size() const{
        return values.size();
    }

    bool empty() const{
        return values.empty();
    }

    std::size_t count(T value) const{
        return values.count(value);
    }

    void insert(T value){
        const auto right = values.lower_bound(value);
        const auto left = right == values.begin() ? values.end() : std::prev(right);
        if(left != values.end() && right != values.end()) erase_xor(left, right);
        const auto inserted = values.insert(right, value);
        if(left != values.end()) adjacent_xors.insert(*left ^ *inserted);
        if(right != values.end()) adjacent_xors.insert(*inserted ^ *right);
    }

    bool erase(T value){
        const auto erased = values.find(value);
        if(erased == values.end()) return false;
        const auto right = std::next(erased);
        const auto left = erased == values.begin() ? values.end() : std::prev(erased);
        if(left != values.end()) erase_xor(left, erased);
        if(right != values.end()) erase_xor(erased, right);
        values.erase(erased);
        if(left != values.end() && right != values.end()) adjacent_xors.insert(*left ^ *right);
        return true;
    }

    T minimum_pair_xor() const{
        if(values.size() < 2)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: fewer than two values "
                "(minimum_pair_xor)."
            );
        }
        return *adjacent_xors.begin();
    }
};
