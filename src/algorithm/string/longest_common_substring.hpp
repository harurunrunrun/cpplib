#ifndef CPPLIB_SRC_ALGORITHM_STRING_LONGEST_COMMON_SUBSTRING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_LONGEST_COMMON_SUBSTRING_HPP_INCLUDED

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>

#include "suffix_automaton.hpp"

struct LongestCommonSubstringResult{
    int first_pos;
    int second_pos;
    int length;
};

inline LongestCommonSubstringResult longest_common_substring(const std::string& a, const std::string& b){
    if(a.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
       b.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())){
        throw std::length_error(
            "library assertion fault: input string is too long."
        );
    }
    using suffix_automaton_internal::ByteSuffixAutomaton;
    const ByteSuffixAutomaton automaton(a);
    LongestCommonSubstringResult best{0, 0, 0};
    int state = 0;
    int matched = 0;
    for(int position = 0; position < static_cast<int>(b.size()); ++position){
        const unsigned char byte =
            static_cast<unsigned char>(b[static_cast<std::size_t>(position)]);
        int next = automaton.next(state, byte);
        while(state != 0 && next == -1){
            state = automaton[state].link;
            matched = automaton[state].length;
            next = automaton.next(state, byte);
        }
        if(next == -1){
            state = 0;
            matched = 0;
            continue;
        }
        state = next;
        ++matched;
        if(best.length < matched){
            best.length = matched;
            best.first_pos = automaton[state].first_end - matched + 1;
            best.second_pos = position - matched + 1;
        }
    }
    return best;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_LONGEST_COMMON_SUBSTRING_HPP_INCLUDED
