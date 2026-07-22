#ifndef CPPLIB_SRC_ALGORITHM_OTHER_RECONSTRUCT_PERMUTATION_FROM_LARGER_BEFORE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_RECONSTRUCT_PERMUTATION_FROM_LARGER_BEFORE_HPP_INCLUDED

#include <memory>
#include <stdexcept>
#include <vector>

#include "../../structure/tree/integer_set.hpp"

template<int MAX_SIZE = 200000>
std::vector<int> reconstruct_permutation_from_larger_before(
    const std::vector<unsigned int>& larger_before
){
    const int size = static_cast<int>(larger_before.size());
    if(size > MAX_SIZE)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: size exceeds MAX_SIZE "
            "(reconstruct_permutation_from_larger_before)."
        );
    }
    auto available = std::make_unique<Int_Set<unsigned int, static_cast<unsigned int>(MAX_SIZE + 1)>>();
    for(int value = 1; value <= size; ++value){
        available->insert(static_cast<unsigned int>(value));
    }
    std::vector<int> answer(larger_before.size());
    for(int index = size - 1; index >= 0; --index){
        const auto value = available->kth_le(
            static_cast<unsigned int>(size),
            larger_before[static_cast<std::size_t>(index)]
        );
        if(!value)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid inversion sequence "
                "(reconstruct_permutation_from_larger_before)."
            );
        }
        answer[static_cast<std::size_t>(index)] = static_cast<int>(*value);
        available->erase(*value);
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_RECONSTRUCT_PERMUTATION_FROM_LARGER_BEFORE_HPP_INCLUDED
