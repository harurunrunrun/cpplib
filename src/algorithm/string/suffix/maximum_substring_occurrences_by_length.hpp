#ifndef CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_MAXIMUM_SUBSTRING_OCCURRENCES_BY_LENGTH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_MAXIMUM_SUBSTRING_OCCURRENCES_BY_LENGTH_HPP_INCLUDED

#include <limits>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "suffix_automaton.hpp"

template<int MAX_LENGTH, int ALPHABET = 26, char OFFSET = 'a'>
std::vector<long long> maximum_substring_occurrences_by_length(
    std::string_view text
){
    static_assert(MAX_LENGTH >= 0);
    static_assert(MAX_LENGTH <= (std::numeric_limits<int>::max() - 1) / 2);
    if(text.size() > static_cast<std::size_t>(MAX_LENGTH))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: capacity exceeded "
            "(maximum_substring_occurrences_by_length)."
        );
    }

    auto automaton = std::make_unique<
        SuffixAutomaton<ALPHABET, MAX_LENGTH * 2 + 1, OFFSET>
    >();
    for(char character: text) automaton->extend(character);
    return automaton->maximum_occurrences_by_length();
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_MAXIMUM_SUBSTRING_OCCURRENCES_BY_LENGTH_HPP_INCLUDED
