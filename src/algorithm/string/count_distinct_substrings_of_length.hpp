#ifndef CPPLIB_SRC_ALGORITHM_STRING_COUNT_DISTINCT_SUBSTRINGS_OF_LENGTH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_COUNT_DISTINCT_SUBSTRINGS_OF_LENGTH_HPP_INCLUDED

#include <cstddef>
#include <string>

#include "suffix_array.hpp"
#include "suffix_automaton.hpp"

inline long long count_distinct_substrings_of_length(
    const std::string& sequence,
    int length
){
    if(length <= 0 ||
       sequence.size() < static_cast<std::size_t>(length)) return 0;
    const suffix_automaton_internal::ByteSuffixAutomaton automaton(sequence);
    long long result = 0;
    for(int state = 1; state < automaton.size(); ++state){
        const auto& current = automaton[state];
        if(automaton[current.link].length < length &&
           length <= current.length) ++result;
    }
    return result;
}

template<class Sequence>
long long count_distinct_substrings_of_length(
    const Sequence& sequence,
    int length
){
    const int size = static_cast<int>(sequence.size());
    if(length <= 0 || size < length) return 0;

    const std::vector<int> sa = suffix_array(sequence);
    const std::vector<int> lcp = lcp_array(sequence, sa);
    long long result = 0;
    bool previous_suffix_is_long_enough = false;
    for(int rank = 0; rank < size; rank++){
        const bool long_enough = size - sa[static_cast<std::size_t>(rank)] >= length;
        if(long_enough && (!previous_suffix_is_long_enough
            || lcp[static_cast<std::size_t>(rank - 1)] < length)){
            result++;
        }
        previous_suffix_is_long_enough = long_enough;
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_COUNT_DISTINCT_SUBSTRINGS_OF_LENGTH_HPP_INCLUDED
