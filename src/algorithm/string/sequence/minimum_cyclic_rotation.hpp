#ifndef CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_MINIMUM_CYCLIC_ROTATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_MINIMUM_CYCLIC_ROTATION_HPP_INCLUDED

#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <string_view>

template<std::random_access_iterator Iterator, class Compare = std::less<>>
std::size_t minimum_cyclic_rotation_index(
    Iterator first,
    Iterator last,
    Compare compare = Compare{}
){
    const auto difference = last - first;
    if(difference < 0)[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: invalid range "
            "(minimum_cyclic_rotation_index)."
        );
    }

    const std::size_t size = static_cast<std::size_t>(difference);
    if(size == 0) return 0;

    std::size_t left = 0;
    std::size_t right = 1;
    std::size_t matched = 0;
    while(left < size && right < size && matched < size){
        const auto& left_value = first[static_cast<decltype(difference)>(
            (left + matched) % size
        )];
        const auto& right_value = first[static_cast<decltype(difference)>(
            (right + matched) % size
        )];

        if(compare(left_value, right_value)){
            right += matched + 1;
            if(right == left) ++right;
            matched = 0;
        }else if(compare(right_value, left_value)){
            left += matched + 1;
            if(left == right) ++left;
            matched = 0;
        }else{
            ++matched;
        }
    }
    return left < right ? left : right;
}

inline std::size_t minimum_cyclic_rotation_index(std::string_view text){
    return minimum_cyclic_rotation_index(text.begin(), text.end());
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_MINIMUM_CYCLIC_ROTATION_HPP_INCLUDED
