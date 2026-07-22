#ifndef CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_PREFIX_FUNCTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_PREFIX_FUNCTION_HPP_INCLUDED

#include <cstddef>
#include <numeric>
#include <vector>

template<class Sequence>
std::vector<int> prefix_function(const Sequence& sequence){
    const int n = static_cast<int>(sequence.size());
    std::vector<int> result(static_cast<std::size_t>(n));
    for(int i = 1; i < n; i++){
        int length = result[static_cast<std::size_t>(i - 1)];
        while(length > 0 && !(sequence[static_cast<std::size_t>(i)] ==
            sequence[static_cast<std::size_t>(length)])){
            length = result[static_cast<std::size_t>(length - 1)];
        }
        if(sequence[static_cast<std::size_t>(i)] ==
            sequence[static_cast<std::size_t>(length)]){
            length++;
        }
        result[static_cast<std::size_t>(i)] = length;
    }
    return result;
}

template<class Text, class Pattern>
std::vector<int> kmp_search(const Text& text, const Pattern& pattern){
    const int text_size = static_cast<int>(text.size());
    const int pattern_size = static_cast<int>(pattern.size());
    if(pattern_size == 0){
        std::vector<int> result(static_cast<std::size_t>(text_size) + 1);
        std::iota(result.begin(), result.end(), 0);
        return result;
    }

    const std::vector<int> failure = prefix_function(pattern);
    std::vector<int> result;
    int matched = 0;
    for(int i = 0; i < text_size; i++){
        while(matched > 0 && !(text[static_cast<std::size_t>(i)] ==
            pattern[static_cast<std::size_t>(matched)])){
            matched = failure[static_cast<std::size_t>(matched - 1)];
        }
        if(text[static_cast<std::size_t>(i)] == pattern[static_cast<std::size_t>(matched)]){
            matched++;
        }
        if(matched == pattern_size){
            result.push_back(i + 1 - pattern_size);
            matched = failure[static_cast<std::size_t>(matched - 1)];
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_PREFIX_FUNCTION_HPP_INCLUDED
