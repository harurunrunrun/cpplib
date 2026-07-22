#ifndef CPPLIB_SRC_STRUCTURE_STRING_LEXICOGRAPHICAL_DISTINCT_SUBSTRINGS_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_STRING_LEXICOGRAPHICAL_DISTINCT_SUBSTRINGS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../algorithm/string/suffix_array.hpp"

class LexicographicalDistinctSubstrings{
    std::string text_;
    std::vector<int> suffixes_;
    std::vector<int> lcp_;
    std::vector<long long> cumulative_count_;

public:
    explicit LexicographicalDistinctSubstrings(std::string text):
        text_(std::move(text))
    {
        if(text_.size() > static_cast<std::size_t>(
            std::numeric_limits<int>::max()
        ))[[unlikely]]{
            throw std::length_error("lexicographical text is too long");
        }
        suffixes_ = suffix_array(text_);
        lcp_ = lcp_array(text_, suffixes_);
        cumulative_count_.resize(text_.size());
        const int size = static_cast<int>(text_.size());
        long long total = 0;
        for(int index = 0; index < size; ++index){
            const int shared = index == 0
                ? 0
                : lcp_[static_cast<std::size_t>(index - 1)];
            const long long added = size -
                suffixes_[static_cast<std::size_t>(index)] - shared;
            if(total > std::numeric_limits<long long>::max() - added)[[unlikely]]{
                throw std::overflow_error(
                    "lexicographical distinct substring count overflow"
                );
            }
            total += added;
            cumulative_count_[static_cast<std::size_t>(index)] = total;
        }
    }

    long long count() const{
        return cumulative_count_.empty() ? 0 : cumulative_count_.back();
    }

    std::optional<std::string> kth(long long one_based_rank) const{
        if(one_based_rank <= 0 || count() < one_based_rank){
            return std::nullopt;
        }
        const auto iterator = std::lower_bound(
            cumulative_count_.begin(),
            cumulative_count_.end(),
            one_based_rank
        );
        const std::size_t index = static_cast<std::size_t>(
            iterator - cumulative_count_.begin()
        );
        const long long preceding = index == 0
            ? 0
            : cumulative_count_[index - 1];
        const int shared = index == 0 ? 0 : lcp_[index - 1];
        const std::size_t length = static_cast<std::size_t>(
            shared + one_based_rank - preceding
        );
        return text_.substr(
            static_cast<std::size_t>(suffixes_[index]), length
        );
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_STRING_LEXICOGRAPHICAL_DISTINCT_SUBSTRINGS_HPP_INCLUDED
