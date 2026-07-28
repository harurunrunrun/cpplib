#ifndef CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_LONGEST_COMMON_EXTENSION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_LONGEST_COMMON_EXTENSION_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include "suffix_array.hpp"

struct LongestCommonExtension{
    int text_size;
    std::vector<int> suffixes;
    std::vector<int> rank;
    std::vector<int> logarithm;
    std::vector<std::vector<int>> sparse_table;

    explicit LongestCommonExtension(const std::string& text)
        : text_size(static_cast<int>(text.size())),
          suffixes(suffix_array(text)),
          rank(static_cast<std::size_t>(text_size), 0){
        for(int index = 0; index < text_size; ++index){
            rank[static_cast<std::size_t>(
                suffixes[static_cast<std::size_t>(index)]
            )] = index;
        }
        const std::vector<int> lcp = lcp_array(text, suffixes);
        logarithm.assign(lcp.size() + 1, 0);
        for(std::size_t length = 2; length < logarithm.size(); ++length){
            logarithm[length] = logarithm[length / 2] + 1;
        }
        const int levels = lcp.empty()
            ? 0 : logarithm[lcp.size()] + 1;
        sparse_table.resize(static_cast<std::size_t>(levels));
        if(!lcp.empty()) sparse_table[0] = lcp;
        for(int level = 1; level < levels; ++level){
            const std::size_t length =
                lcp.size() - (std::size_t{1} << level) + 1;
            sparse_table[static_cast<std::size_t>(level)]
                .resize(length);
            for(std::size_t index = 0; index < length; ++index){
                sparse_table[static_cast<std::size_t>(level)][index] =
                    std::min(
                        sparse_table[static_cast<std::size_t>(level - 1)]
                            [index],
                        sparse_table[static_cast<std::size_t>(level - 1)]
                            [index + (std::size_t{1} << (level - 1))]
                    );
            }
        }
    }

    int lce(int first, int second) const{
        if(first < 0 || text_size < first
            || second < 0 || text_size < second)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(LongestCommonExtension::lce)."
            );
        }
        if(first == second) return text_size - first;
        if(first == text_size || second == text_size) return 0;
        int left = rank[static_cast<std::size_t>(first)];
        int right = rank[static_cast<std::size_t>(second)];
        if(left > right) std::swap(left, right);
        const int length = right - left;
        const int level = logarithm[static_cast<std::size_t>(length)];
        return std::min(
            sparse_table[static_cast<std::size_t>(level)]
                [static_cast<std::size_t>(left)],
            sparse_table[static_cast<std::size_t>(level)]
                [static_cast<std::size_t>(
                    right - (1 << level)
                )]
        );
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_LONGEST_COMMON_EXTENSION_HPP_INCLUDED
