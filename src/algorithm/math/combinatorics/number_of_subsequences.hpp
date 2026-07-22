#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_NUMBER_OF_SUBSEQUENCES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_NUMBER_OF_SUBSEQUENCES_HPP_INCLUDED

#include <cstdint>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

template<class T, class Hash = std::hash<T>, class Equal = std::equal_to<T>>
std::uint32_t number_of_distinct_subsequences(
    const std::vector<T>& values, std::uint32_t modulus,
    const Hash& hash = Hash(), const Equal& equal = Equal()
){
    if(modulus == 0){
        throw std::invalid_argument(
            "number_of_distinct_subsequences requires modulus > 0"
        );
    }
    std::unordered_map<T, std::uint32_t, Hash, Equal> previous(
        0, hash, equal
    );
    previous.reserve(values.size());
    std::uint32_t including_empty = 1 % modulus;
    for(const T& value: values){
        const auto iterator = previous.find(value);
        const std::uint32_t duplicated =
            iterator == previous.end() ? 0 : iterator->second;
        previous[value] = including_empty;
        const std::uint64_t doubled =
            static_cast<std::uint64_t>(including_empty) * 2;
        including_empty = static_cast<std::uint32_t>(
            (doubled + modulus - duplicated) % modulus
        );
    }
    return static_cast<std::uint32_t>(
        (static_cast<std::uint64_t>(including_empty) + modulus - 1) % modulus
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_NUMBER_OF_SUBSEQUENCES_HPP_INCLUDED
