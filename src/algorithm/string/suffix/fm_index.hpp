#ifndef CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_FM_INDEX_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_FM_INDEX_HPP_INCLUDED

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "suffix_array.hpp"

struct FMIndex{
    static constexpr int alphabet_size = 257;

    std::vector<int> suffixes;
    std::vector<int> bwt;
    std::array<int, alphabet_size + 1> cumulative{};
    std::array<std::vector<std::uint64_t>, alphabet_size> bit_words;
    std::array<std::vector<int>, alphabet_size> word_prefix;

    explicit FMIndex(const std::string& text){
        std::vector<int> encoded;
        encoded.reserve(text.size() + 1);
        for(unsigned char symbol: text){
            encoded.push_back(static_cast<int>(symbol) + 1);
        }
        encoded.push_back(0);
        suffixes = suffix_array(encoded);
        const int size = static_cast<int>(encoded.size());
        bwt.resize(static_cast<std::size_t>(size));
        std::array<int, alphabet_size> frequency{};
        const std::size_t word_count =
            (static_cast<std::size_t>(size) + 63) / 64;
        for(auto& words: bit_words) words.assign(word_count, 0);
        for(int row = 0; row < size; ++row){
            const int position = suffixes[static_cast<std::size_t>(row)];
            const int symbol =
                position == 0 ? 0 : encoded[static_cast<std::size_t>(
                    position - 1
                )];
            bwt[static_cast<std::size_t>(row)] = symbol;
            ++frequency[static_cast<std::size_t>(symbol)];
            bit_words[static_cast<std::size_t>(symbol)]
                [static_cast<std::size_t>(row / 64)]
                |= std::uint64_t{1} << (row % 64);
        }
        for(int symbol = 0; symbol < alphabet_size; ++symbol){
            cumulative[static_cast<std::size_t>(symbol + 1)] =
                cumulative[static_cast<std::size_t>(symbol)]
                + frequency[static_cast<std::size_t>(symbol)];
            auto& prefix = word_prefix[static_cast<std::size_t>(symbol)];
            prefix.assign(word_count + 1, 0);
            for(std::size_t word = 0; word < word_count; ++word){
                prefix[word + 1] = prefix[word]
                    + std::popcount(
                        bit_words[static_cast<std::size_t>(symbol)][word]
                    );
            }
        }
    }

    int rank(int symbol, int end) const{
        if(symbol < 0 || alphabet_size <= symbol
            || end < 0 || static_cast<int>(bwt.size()) < end)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (FMIndex::rank)."
            );
        }
        const std::size_t word = static_cast<std::size_t>(end / 64);
        const int bit = end % 64;
        int result =
            word_prefix[static_cast<std::size_t>(symbol)][word];
        if(bit != 0 && word
            < bit_words[static_cast<std::size_t>(symbol)].size()){
            result += std::popcount(
                bit_words[static_cast<std::size_t>(symbol)][word]
                & ((std::uint64_t{1} << bit) - 1)
            );
        }
        return result;
    }

    std::pair<int, int> equal_range(std::string_view pattern) const{
        int left = 0;
        int right = static_cast<int>(suffixes.size());
        for(auto iterator = pattern.rbegin();
            iterator != pattern.rend() && left < right;
            ++iterator){
            const int symbol =
                static_cast<unsigned char>(*iterator) + 1;
            left = cumulative[static_cast<std::size_t>(symbol)]
                + rank(symbol, left);
            right = cumulative[static_cast<std::size_t>(symbol)]
                + rank(symbol, right);
        }
        return {left, right};
    }

    int count(std::string_view pattern) const{
        const auto [left, right] = equal_range(pattern);
        return right - left;
    }

    std::vector<int> locate(std::string_view pattern) const{
        const auto [left, right] = equal_range(pattern);
        std::vector<int> result;
        result.reserve(static_cast<std::size_t>(right - left));
        for(int row = left; row < right; ++row){
            const int position = suffixes[static_cast<std::size_t>(row)];
            result.push_back(position);
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_FM_INDEX_HPP_INCLUDED
