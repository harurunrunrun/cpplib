#ifndef CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_COUNT_DISTINCT_SUBSTRINGS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_COUNT_DISTINCT_SUBSTRINGS_HPP_INCLUDED

#include <string>

#include "suffix_array.hpp"
#include "suffix_automaton.hpp"

inline long long count_distinct_substrings(const std::string& sequence){
    const suffix_automaton_internal::ByteSuffixAutomaton automaton(sequence);
    long long result = 0;
    for(int state = 1; state < automaton.size(); ++state){
        const auto& current = automaton[state];
        result += current.length - automaton[current.link].length;
    }
    return result;
}

template<class Sequence>
long long count_distinct_substrings(const Sequence& sequence){
    const std::vector<int> sa = suffix_array(sequence);
    const std::vector<int> lcp = lcp_array(sequence, sa);
    const long long size = static_cast<long long>(sequence.size());
    long long result = size * (size + 1) / 2;
    for(int common_prefix_length: lcp){
        result -= common_prefix_length;
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_COUNT_DISTINCT_SUBSTRINGS_HPP_INCLUDED
