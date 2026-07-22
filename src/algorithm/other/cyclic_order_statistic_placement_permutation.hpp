#ifndef CPPLIB_SRC_ALGORITHM_OTHER_CYCLIC_ORDER_STATISTIC_PLACEMENT_PERMUTATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_CYCLIC_ORDER_STATISTIC_PLACEMENT_PERMUTATION_HPP_INCLUDED

#include <memory>
#include <stdexcept>
#include <vector>

#include "../../structure/tree/integer_set.hpp"

template<int MAX_SIZE = 20000>
std::vector<int> cyclic_order_statistic_placement_permutation(int size){
    static_assert(0 < MAX_SIZE);
    if(size < 0 || MAX_SIZE < size)[[unlikely]]{
        throw std::out_of_range(
            "cyclic_order_statistic_placement_permutation size is out of range"
        );
    }

    auto available_positions = std::make_unique<
        Int_Set<unsigned int, static_cast<unsigned int>(MAX_SIZE)>
    >();
    for(int position = 0; position < size; ++position){
        available_positions->insert(static_cast<unsigned int>(position));
    }

    std::vector<int> permutation(static_cast<std::size_t>(size));
    long long rank = 0;
    int remaining = size;
    for(int value = 1; value <= size; ++value){
        rank = (rank + value) % remaining;
        const auto position = available_positions->kth_ge(
            0U, static_cast<unsigned int>(rank)
        );
        if(!position)[[unlikely]]{
            throw std::runtime_error(
                "cyclic_order_statistic_placement_permutation "
                "order statistic failed"
            );
        }
        permutation[static_cast<std::size_t>(*position)] = value;
        available_positions->erase(*position);
        --remaining;
        if(remaining != 0) rank %= remaining;
    }
    return permutation;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_CYCLIC_ORDER_STATISTIC_PLACEMENT_PERMUTATION_HPP_INCLUDED
