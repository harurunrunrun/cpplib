#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class LongestCommonSuffixDictionary{
    struct Entry{
        std::string reversed_text;
        int text_id;
    };

    struct TwoMinimum{
        int first = -1;
        int second = -1;
    };

    std::vector<std::string> texts;
    std::vector<Entry> entries;
    std::size_t segment_size = 1;
    std::vector<TwoMinimum> segment_tree;

    static void add_candidate(TwoMinimum& result, int value){
        if(value < 0 || value == result.first || value == result.second) return;
        if(result.first < 0 || value < result.first){
            result.second = result.first;
            result.first = value;
        }else if(result.second < 0 || value < result.second){
            result.second = value;
        }
    }

    static TwoMinimum merge(TwoMinimum left, const TwoMinimum& right){
        add_candidate(left, right.first);
        add_candidate(left, right.second);
        return left;
    }

    static int longest_common_prefix(
        std::string_view left,
        std::string_view right
    ){
        int length = 0;
        const int limit = static_cast<int>(std::min(left.size(), right.size()));
        while(length < limit &&
              left[static_cast<std::size_t>(length)] ==
                  right[static_cast<std::size_t>(length)]){
            ++length;
        }
        return length;
    }

    static int compare_with_prefix(
        std::string_view text,
        std::string_view prefix
    ){
        const int comparison = text.compare(0, prefix.size(), prefix);
        if(comparison < 0) return -1;
        if(comparison > 0) return 1;
        return 0;
    }

    std::size_t prefix_lower_bound(std::string_view prefix) const{
        std::size_t lower = 0;
        std::size_t upper = entries.size();
        while(lower < upper){
            const std::size_t middle = lower + (upper - lower) / 2;
            if(compare_with_prefix(entries[middle].reversed_text, prefix) < 0){
                lower = middle + 1;
            }else{
                upper = middle;
            }
        }
        return lower;
    }

    std::size_t prefix_upper_bound(std::string_view prefix) const{
        std::size_t lower = 0;
        std::size_t upper = entries.size();
        while(lower < upper){
            const std::size_t middle = lower + (upper - lower) / 2;
            if(compare_with_prefix(entries[middle].reversed_text, prefix) <= 0){
                lower = middle + 1;
            }else{
                upper = middle;
            }
        }
        return lower;
    }

    TwoMinimum range_minimum(std::size_t lower, std::size_t upper) const{
        TwoMinimum left;
        TwoMinimum right;
        lower += segment_size;
        upper += segment_size;
        while(lower < upper){
            if(lower & 1U) left = merge(left, segment_tree[lower++]);
            if(upper & 1U) right = merge(segment_tree[--upper], right);
            lower >>= 1U;
            upper >>= 1U;
        }
        return merge(left, right);
    }

public:
    LongestCommonSuffixDictionary(){
        segment_tree.assign(2, TwoMinimum());
    }

    explicit LongestCommonSuffixDictionary(std::vector<std::string> dictionary){
        build(std::move(dictionary));
    }

    void build(std::vector<std::string> dictionary){
        if(dictionary.size() > static_cast<std::size_t>(
            std::numeric_limits<int>::max()
        ))[[unlikely]]{
            throw std::length_error(
                "LongestCommonSuffixDictionary has too many entries"
            );
        }
        for(const auto& text: dictionary){
            if(text.size() > static_cast<std::size_t>(
                std::numeric_limits<int>::max()
            ))[[unlikely]]{
                throw std::length_error(
                    "LongestCommonSuffixDictionary entry is too long"
                );
            }
        }
        std::sort(dictionary.begin(), dictionary.end());
        if(std::adjacent_find(dictionary.begin(), dictionary.end()) !=
           dictionary.end())[[unlikely]]{
            throw std::invalid_argument(
                "LongestCommonSuffixDictionary requires distinct entries"
            );
        }

        texts = std::move(dictionary);
        entries.clear();
        entries.reserve(texts.size());
        for(int text_id = 0; text_id < static_cast<int>(texts.size());
            ++text_id){
            entries.push_back({
                std::string(
                    texts[static_cast<std::size_t>(text_id)].rbegin(),
                    texts[static_cast<std::size_t>(text_id)].rend()
                ),
                text_id,
            });
        }
        std::sort(
            entries.begin(), entries.end(),
            [](const Entry& left, const Entry& right){
                return left.reversed_text < right.reversed_text;
            }
        );

        segment_size = 1;
        while(segment_size < entries.size()){
            if(segment_size > std::numeric_limits<std::size_t>::max() / 2)
                [[unlikely]]{
                throw std::length_error(
                    "LongestCommonSuffixDictionary segment tree is too large"
                );
            }
            segment_size <<= 1U;
        }
        if(segment_size > std::vector<TwoMinimum>().max_size() / 2)
            [[unlikely]]{
            throw std::length_error(
                "LongestCommonSuffixDictionary segment tree is too large"
            );
        }
        segment_tree.assign(2 * segment_size, TwoMinimum());
        for(std::size_t index = 0; index < entries.size(); ++index){
            segment_tree[segment_size + index].first = entries[index].text_id;
        }
        for(std::size_t index = segment_size; index-- > 1;){
            segment_tree[index] = merge(
                segment_tree[index << 1U],
                segment_tree[index << 1U | 1U]
            );
        }
    }

    int size() const{
        return static_cast<int>(texts.size());
    }

    bool empty() const{
        return texts.empty();
    }

    std::optional<std::string_view> best_match(
        std::string_view query
    ) const{
        if(query.size() > static_cast<std::size_t>(
            std::numeric_limits<int>::max()
        ))[[unlikely]]{
            throw std::length_error(
                "LongestCommonSuffixDictionary query is too long"
            );
        }
        if(entries.empty()) return std::nullopt;

        const std::string reversed_query(query.rbegin(), query.rend());
        const auto position_iterator = std::lower_bound(
            entries.begin(), entries.end(), reversed_query,
            [](const Entry& entry, const std::string& value){
                return entry.reversed_text < value;
            }
        );
        const std::size_t position = static_cast<std::size_t>(
            position_iterator - entries.begin()
        );

        int best_length = -1;
        const std::size_t lower_neighbor = position == 0 ? 0 : position - 1;
        const std::size_t upper_neighbor = std::min(
            entries.size(), position + 2
        );
        for(std::size_t index = lower_neighbor; index < upper_neighbor;
            ++index){
            const int text_id = entries[index].text_id;
            if(std::string_view(texts[static_cast<std::size_t>(text_id)]) ==
               query){
                continue;
            }
            best_length = std::max(
                best_length,
                longest_common_prefix(
                    reversed_query, entries[index].reversed_text
                )
            );
        }
        if(best_length < 0) return std::nullopt;

        const std::string_view prefix(
            reversed_query.data(), static_cast<std::size_t>(best_length)
        );
        const std::size_t lower = prefix_lower_bound(prefix);
        const std::size_t upper = prefix_upper_bound(prefix);
        const TwoMinimum candidates = range_minimum(lower, upper);
        for(const int text_id: {candidates.first, candidates.second}){
            if(text_id >= 0 &&
               std::string_view(texts[static_cast<std::size_t>(text_id)]) !=
                   query){
                return std::string_view(
                    texts[static_cast<std::size_t>(text_id)]
                );
            }
        }
        return std::nullopt;
    }
};
