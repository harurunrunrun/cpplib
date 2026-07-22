#ifndef CPPLIB_SRC_ALGORITHM_STRING_DISTINCT_SUBSTRINGS_BY_FIRST_BYTE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_DISTINCT_SUBSTRINGS_BY_FIRST_BYTE_HPP_INCLUDED

#include <array>
#include <string>
#include <vector>

#include "suffix_automaton.hpp"

inline std::array<long long, 256>
distinct_substrings_by_first_byte(const std::string& text){
    using suffix_automaton_internal::ByteSuffixAutomaton;
    std::array<long long, 256> result{};
    const ByteSuffixAutomaton automaton(text);
    const std::vector<int> order = automaton.length_order();
    std::vector<long long> path_count(
        static_cast<std::size_t>(automaton.size()), 1
    );
    for(std::size_t position = order.size(); position > 1; --position){
        const int state = order[position - 1];
        long long& count = path_count[static_cast<std::size_t>(state)];
        for(const auto& [byte, target]: automaton[state].next){
            static_cast<void>(byte);
            count += path_count[static_cast<std::size_t>(target)];
        }
    }
    for(const auto& [byte, target]: automaton[0].next){
        result[byte] = path_count[static_cast<std::size_t>(target)];
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_DISTINCT_SUBSTRINGS_BY_FIRST_BYTE_HPP_INCLUDED
