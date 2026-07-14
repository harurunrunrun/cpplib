#pragma once

#include <algorithm>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class PerfectRhymeDictionary{
    struct Entry{
        std::string reversed;
        int word_id;
    };

    struct TwoMinimum{
        int first = -1;
        int second = -1;
    };

    std::vector<std::string> words;
    std::vector<Entry> entries;
    int segment_size = 1;
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
        while(length < limit && left[length] == right[length]) length++;
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

    int prefix_lower_bound(std::string_view prefix) const{
        int lower = 0;
        int upper = static_cast<int>(entries.size());
        while(lower < upper){
            const int middle = (lower + upper) / 2;
            if(compare_with_prefix(entries[middle].reversed, prefix) < 0){
                lower = middle + 1;
            }else{
                upper = middle;
            }
        }
        return lower;
    }

    int prefix_upper_bound(std::string_view prefix) const{
        int lower = 0;
        int upper = static_cast<int>(entries.size());
        while(lower < upper){
            const int middle = (lower + upper) / 2;
            if(compare_with_prefix(entries[middle].reversed, prefix) <= 0){
                lower = middle + 1;
            }else{
                upper = middle;
            }
        }
        return lower;
    }

    TwoMinimum range_minimum(int lower, int upper) const{
        TwoMinimum left, right;
        lower += segment_size;
        upper += segment_size;
        while(lower < upper){
            if(lower & 1) left = merge(left, segment_tree[lower++]);
            if(upper & 1) right = merge(segment_tree[--upper], right);
            lower >>= 1;
            upper >>= 1;
        }
        return merge(left, right);
    }

public:
    PerfectRhymeDictionary(){
        segment_tree.assign(2, TwoMinimum());
    }

    explicit PerfectRhymeDictionary(std::vector<std::string> dictionary){
        build(std::move(dictionary));
    }

    void build(std::vector<std::string> dictionary){
        std::sort(dictionary.begin(), dictionary.end());
        if(std::adjacent_find(dictionary.begin(), dictionary.end())
            != dictionary.end())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: duplicate word "
                "(PerfectRhymeDictionary::build)."
            );
        }

        words = std::move(dictionary);
        entries.clear();
        entries.reserve(words.size());
        for(int word_id = 0; word_id < static_cast<int>(words.size()); word_id++){
            entries.push_back({
                std::string(words[word_id].rbegin(), words[word_id].rend()),
                word_id,
            });
        }
        std::sort(entries.begin(), entries.end(), [](const Entry& left, const Entry& right){
            return left.reversed < right.reversed;
        });

        segment_size = 1;
        while(segment_size < static_cast<int>(entries.size())) segment_size <<= 1;
        segment_tree.assign(2 * segment_size, TwoMinimum());
        for(int i = 0; i < static_cast<int>(entries.size()); i++){
            segment_tree[segment_size + i].first = entries[i].word_id;
        }
        for(int i = segment_size - 1; i > 0; i--){
            segment_tree[i] = merge(
                segment_tree[i << 1],
                segment_tree[i << 1 | 1]
            );
        }
    }

    int size() const{ return static_cast<int>(words.size()); }
    bool empty() const{ return words.empty(); }

    std::optional<std::string_view> perfect_rhyme(std::string_view word) const{
        if(entries.empty()) return std::nullopt;

        const std::string reversed(word.rbegin(), word.rend());
        const auto position_iterator = std::lower_bound(
            entries.begin(),
            entries.end(),
            reversed,
            [](const Entry& entry, const std::string& value){
                return entry.reversed < value;
            }
        );
        const int position = static_cast<int>(position_iterator - entries.begin());

        int best_length = -1;
        for(int index = position - 1; index <= position + 1; index++){
            if(index < 0 || static_cast<int>(entries.size()) <= index) continue;
            const int word_id = entries[index].word_id;
            if(std::string_view(words[word_id]) == word) continue;
            best_length = std::max(
                best_length,
                longest_common_prefix(reversed, entries[index].reversed)
            );
        }
        if(best_length < 0) return std::nullopt;

        const std::string_view prefix(reversed.data(), best_length);
        const int lower = prefix_lower_bound(prefix);
        const int upper = prefix_upper_bound(prefix);
        const TwoMinimum candidates = range_minimum(lower, upper);
        for(int word_id: {candidates.first, candidates.second}){
            if(word_id >= 0 && std::string_view(words[word_id]) != word){
                return std::string_view(words[word_id]);
            }
        }
        return std::nullopt;
    }
};
