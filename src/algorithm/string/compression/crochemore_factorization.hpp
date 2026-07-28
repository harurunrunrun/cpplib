#ifndef CPPLIB_SRC_ALGORITHM_STRING_COMPRESSION_CROCHEMORE_FACTORIZATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_COMPRESSION_CROCHEMORE_FACTORIZATION_HPP_INCLUDED

#include <cstddef>
#include <iterator>
#include <set>
#include <string>
#include <vector>

#include "../suffix/longest_common_extension.hpp"

struct CrochemoreFactor{
    int begin;
    int length;
    int source;
};

inline std::vector<CrochemoreFactor> crochemore_factorization(
    const std::string& text
){
    const int size = static_cast<int>(text.size());
    if(size == 0) return {};
    const LongestCommonExtension lce(text);
    std::vector<int> rank(static_cast<std::size_t>(size));
    for(int order = 0; order < size; ++order){
        rank[static_cast<std::size_t>(
            lce.suffixes[static_cast<std::size_t>(order)]
        )] = order;
    }
    std::set<int> active;
    std::vector<CrochemoreFactor> result;
    for(int position = 0; position < size;){
        int source = -1;
        int length = 0;
        const int current_rank = rank[static_cast<std::size_t>(position)];
        const auto inspect = [&](std::set<int>::const_iterator iterator){
            if(iterator == active.end()) return;
            const int candidate =
                lce.suffixes[static_cast<std::size_t>(*iterator)];
            const int common = lce.lce(position, candidate);
            if(common > length
                || (common == length
                    && (source == -1 || candidate < source))){
                length = common;
                source = candidate;
            }
        };
        const auto next = active.lower_bound(current_rank);
        inspect(next);
        if(next != active.begin()) inspect(std::prev(next));
        if(length == 0){
            length = 1;
            source = -1;
        }
        result.push_back({position, length, source});
        for(int index = position; index < position + length; ++index){
            active.insert(rank[static_cast<std::size_t>(index)]);
        }
        position += length;
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_COMPRESSION_CROCHEMORE_FACTORIZATION_HPP_INCLUDED
