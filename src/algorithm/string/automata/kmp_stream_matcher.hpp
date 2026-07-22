#ifndef CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_KMP_STREAM_MATCHER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_KMP_STREAM_MATCHER_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

#include "prefix_function.hpp"

template<class Pattern>
struct KmpStreamMatcher{
private:
    Pattern pattern;
    std::vector<int> failure;
    int matched = 0;
    long long processed = 0;

public:
    explicit KmpStreamMatcher(Pattern pattern_):
        pattern(std::move(pattern_)), failure(prefix_function(pattern)){
        if(pattern.empty())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: pattern must be non-empty (KmpStreamMatcher)."
            );
        }
    }

    int pattern_size() const{ return static_cast<int>(pattern.size()); }
    int matched_length() const{ return matched; }
    long long processed_size() const{ return processed; }

    void reset(){
        matched = 0;
        processed = 0;
    }

    template<class Value>
    long long feed(const Value& value){
        while(matched > 0 && !(value == pattern[static_cast<std::size_t>(matched)])){
            matched = failure[static_cast<std::size_t>(matched - 1)];
        }
        if(value == pattern[static_cast<std::size_t>(matched)]) matched++;
        processed++;
        if(matched != pattern_size()) return -1;

        const long long position = processed - pattern_size();
        matched = failure.back();
        return position;
    }
};

template<class Pattern>
KmpStreamMatcher(Pattern) -> KmpStreamMatcher<Pattern>;

#endif  // CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_KMP_STREAM_MATCHER_HPP_INCLUDED
