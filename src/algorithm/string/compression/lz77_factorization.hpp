#ifndef CPPLIB_SRC_ALGORITHM_STRING_COMPRESSION_LZ77_FACTORIZATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_COMPRESSION_LZ77_FACTORIZATION_HPP_INCLUDED

#include <cstddef>
#include <iterator>
#include <set>
#include <string>
#include <vector>

#include "../suffix/longest_common_extension.hpp"

struct LZ77Factor{
    int source = -1;
    int length = 0;
    unsigned char next_symbol = 0;
    bool has_next_symbol = false;
};

inline std::vector<LZ77Factor> lz77_factorization(const std::string& text){
    const int size = static_cast<int>(text.size());
    if(size == 0) return {};
    const LongestCommonExtension lce(text);
    std::vector<int> rank(static_cast<std::size_t>(size));
    for(int order = 0; order < size; ++order){
        rank[static_cast<std::size_t>(
            lce.suffixes[static_cast<std::size_t>(order)]
        )] = order;
    }
    std::set<int> active_ranks;
    std::vector<LZ77Factor> result;
    for(int position = 0; position < size;){
        const int current_rank = rank[static_cast<std::size_t>(position)];
        int best_source = -1;
        int best_length = 0;
        const auto inspect = [&](std::set<int>::const_iterator iterator){
            if(iterator == active_ranks.end()) return;
            const int source =
                lce.suffixes[static_cast<std::size_t>(*iterator)];
            const int length = lce.lce(position, source);
            if(length > best_length
                || (length == best_length
                    && (best_source == -1 || source < best_source))){
                best_length = length;
                best_source = source;
            }
        };
        const auto next = active_ranks.lower_bound(current_rank);
        inspect(next);
        if(next != active_ranks.begin()) inspect(std::prev(next));
        LZ77Factor factor;
        factor.source = best_source;
        factor.length = best_length;
        const int after_match = position + best_length;
        if(after_match < size){
            factor.has_next_symbol = true;
            factor.next_symbol = static_cast<unsigned char>(
                text[static_cast<std::size_t>(after_match)]
            );
        }
        result.push_back(factor);
        const int advance = best_length + (factor.has_next_symbol ? 1 : 0);
        for(int index = position; index < position + advance; ++index){
            active_ranks.insert(rank[static_cast<std::size_t>(index)]);
        }
        position += advance;
    }
    return result;
}

inline std::string decode_lz77(const std::vector<LZ77Factor>& factors){
    std::string result;
    for(const LZ77Factor& factor: factors){
        if(factor.length != 0){
            const std::size_t source = static_cast<std::size_t>(factor.source);
            for(int offset = 0; offset < factor.length; ++offset){
                result.push_back(result[source + static_cast<std::size_t>(
                    offset
                )]);
            }
        }
        if(factor.has_next_symbol){
            result.push_back(static_cast<char>(factor.next_symbol));
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_COMPRESSION_LZ77_FACTORIZATION_HPP_INCLUDED
