#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#include "../../structure/tree/integer_set.hpp"

template<int MAX_SIZE = 20000>
std::vector<int> card_trick_permutation(int size){
    if(size < 0 || MAX_SIZE < size)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid size (card_trick_permutation)."
        );
    }
    auto positions = std::make_unique<
        Int_Set<unsigned int, static_cast<unsigned int>(MAX_SIZE)>
    >();
    for(int position = 0; position < size; ++position){
        positions->insert(static_cast<unsigned int>(position));
    }
    std::vector<int> answer(static_cast<std::size_t>(size));
    int cursor = 0;
    int remaining = size;
    for(int card = 1; card <= size; ++card){
        cursor = (cursor + card) % remaining;
        const auto position = positions->kth_ge(0U, static_cast<unsigned int>(cursor));
        if(!position)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: order statistic failed "
                "(card_trick_permutation)."
            );
        }
        answer[static_cast<std::size_t>(*position)] = card;
        positions->erase(*position);
        --remaining;
        if(remaining != 0) cursor %= remaining;
    }
    return answer;
}
