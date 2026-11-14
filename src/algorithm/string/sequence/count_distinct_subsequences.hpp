#ifndef CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_COUNT_DISTINCT_SUBSEQUENCES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_COUNT_DISTINCT_SUBSEQUENCES_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <stdexcept>
#include <string>

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
    // previous_total[c]: total before the last occurrence of c (0 if none).
    std::array<long long, 256> previous_total{};
    long long total = 1 % modulus;
    for(unsigned char character: text){
        const long long before = total;
        const long long duplicate = previous_total[character];
        total = (static_cast<__int128>(2) * total - duplicate) % modulus;
        if(total < 0) total += modulus;
        previous_total[character] = before;
    }
    return total;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_COUNT_DISTINCT_SUBSEQUENCES_HPP_INCLUDED
