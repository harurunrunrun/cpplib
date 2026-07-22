#ifndef CPPLIB_SRC_ALGORITHM_OTHER_SEQUENCE_RELATIVE_ORDER_INVERSION_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_SEQUENCE_RELATIVE_ORDER_INVERSION_COUNT_HPP_INCLUDED

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "inversion_count.hpp"

template<class Key>
long long relative_order_inversion_count(
    const std::vector<Key>& order,
    const std::vector<Key>& reference
){
    if(order.size() != reference.size())[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: size mismatch "
            "(relative_order_inversion_count)."
        );
    }
    std::unordered_map<Key, int> position;
    position.reserve(reference.size() * 2);
    for(int index = 0; index < static_cast<int>(reference.size()); ++index){
        if(!position.emplace(reference[static_cast<std::size_t>(index)], index).second)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: duplicate key "
                "(relative_order_inversion_count)."
            );
        }
    }
    std::vector<int> sequence;
    sequence.reserve(order.size());
    for(const Key& key: order){
        const auto iterator = position.find(key);
        if(iterator == position.end())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: unknown key "
                "(relative_order_inversion_count)."
            );
        }
        sequence.push_back(iterator->second);
    }
    return inversion_count(sequence);
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_SEQUENCE_RELATIVE_ORDER_INVERSION_COUNT_HPP_INCLUDED
