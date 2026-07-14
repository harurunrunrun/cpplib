#pragma once

#include <algorithm>
#include <limits>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "suffix_automaton.hpp"

template<int MAX_LENGTH, int ALPHABET = 26, char OFFSET = 'a'>
int longest_common_substring_multiple_length(
    std::span<const std::string> texts
){
    static_assert(MAX_LENGTH >= 0);
    static_assert(MAX_LENGTH <= (std::numeric_limits<int>::max() - 1) / 2);
    if(texts.empty()) return 0;

    const auto shortest = std::min_element(
        texts.begin(),
        texts.end(),
        [](const std::string& left, const std::string& right){
            return left.size() < right.size();
        }
    );
    if(shortest->size() > static_cast<std::size_t>(MAX_LENGTH))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: capacity exceeded "
            "(longest_common_substring_multiple_length)."
        );
    }

    auto automaton = std::make_unique<
        SuffixAutomaton<ALPHABET, MAX_LENGTH * 2 + 1, OFFSET>
    >();
    for(char character: *shortest) automaton->extend(character);

    std::vector<std::string_view> additional;
    additional.reserve(texts.size() - 1);
    for(auto iterator = texts.begin(); iterator != texts.end(); ++iterator){
        if(iterator != shortest) additional.emplace_back(*iterator);
    }
    return automaton->longest_common_substring_length(additional);
}
