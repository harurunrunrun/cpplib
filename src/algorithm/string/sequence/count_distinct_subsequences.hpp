#ifndef CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_COUNT_DISTINCT_SUBSEQUENCES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_COUNT_DISTINCT_SUBSEQUENCES_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <string>
#include <unordered_map>

inline long long count_distinct_subsequences(
    const std::string& text,
    long long modulus = 1'000'000'007LL
){
    if(modulus <= 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: nonpositive modulus "
            "(count_distinct_subsequences)."
        );
    }
    std::unordered_map<unsigned char, long long> previous_total;
    long long total = 1 % modulus;
    for(unsigned char character: text){
        const long long before = total;
        const auto iterator = previous_total.find(character);
        const long long duplicate = iterator == previous_total.end() ? 0 : iterator->second;
        total = (static_cast<__int128>(2) * total - duplicate) % modulus;
        if(total < 0) total += modulus;
        previous_total[character] = before;
    }
    return total;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_COUNT_DISTINCT_SUBSEQUENCES_HPP_INCLUDED
