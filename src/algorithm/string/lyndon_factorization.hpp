#ifndef CPPLIB_SRC_ALGORITHM_STRING_LYNDON_FACTORIZATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_LYNDON_FACTORIZATION_HPP_INCLUDED

#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <string_view>
#include <vector>

template<std::random_access_iterator Iterator, class Compare = std::less<>>
std::vector<std::size_t> lyndon_factorization(
    Iterator first,
    Iterator last,
    Compare compare = Compare{}
){
    const auto difference = last - first;
    if(difference < 0)[[unlikely]]{
        throw std::invalid_argument(
            "library assertion fault: invalid range (lyndon_factorization)."
        );
    }
    const std::size_t size = static_cast<std::size_t>(difference);
    std::vector<std::size_t> boundaries{0};

    std::size_t begin = 0;
    while(begin < size){
        std::size_t repeated = begin;
        std::size_t cursor = begin + 1;
        while(cursor < size && !compare(first[cursor], first[repeated])){
            if(compare(first[repeated], first[cursor])) repeated = begin;
            else ++repeated;
            ++cursor;
        }

        const std::size_t factor_length = cursor - repeated;
        do{
            begin += factor_length;
            boundaries.push_back(begin);
        }while(begin <= repeated);
    }
    return boundaries;
}

inline std::vector<std::size_t> lyndon_factorization(std::string_view text){
    return lyndon_factorization(text.begin(), text.end());
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_LYNDON_FACTORIZATION_HPP_INCLUDED
