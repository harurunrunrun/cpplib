#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

inline long long count_encoded_dictionary_segmentations(
    const std::string& encoded_text,
    const std::vector<std::string>& encoded_tokens,
    long long count_limit = std::numeric_limits<long long>::max()
){
    if(count_limit < 0)[[unlikely]]{
        throw std::invalid_argument(
            "count_encoded_dictionary_segmentations negative limit"
        );
    }

    struct Node{
        std::vector<std::pair<unsigned char, int>> next;
        long long multiplicity = 0;
    };
    std::vector<Node> trie(1);

    const auto transition = [&](int state, unsigned char symbol){
        const auto& next = trie[static_cast<std::size_t>(state)].next;
        const auto iterator = std::lower_bound(
            next.begin(), next.end(), symbol,
            [](const auto& edge, unsigned char value){
                return edge.first < value;
            }
        );
        return iterator != next.end() && iterator->first == symbol
            ? iterator->second : -1;
    };

    std::size_t maximum_token_length = 0;
    for(const auto& token: encoded_tokens){
        if(token.empty())[[unlikely]]{
            throw std::invalid_argument(
                "count_encoded_dictionary_segmentations empty token"
            );
        }
        int state = 0;
        for(const unsigned char symbol: token){
            int to = transition(state, symbol);
            if(to == -1){
                if(trie.size() >= static_cast<std::size_t>(
                    std::numeric_limits<int>::max()
                ))[[unlikely]]{
                    throw std::length_error(
                        "count_encoded_dictionary_segmentations "
                        "dictionary is too large"
                    );
                }
                to = static_cast<int>(trie.size());
                auto& next = trie[static_cast<std::size_t>(state)].next;
                const auto position = std::lower_bound(
                    next.begin(), next.end(), symbol,
                    [](const auto& edge, unsigned char value){
                        return edge.first < value;
                    }
                );
                next.insert(position, {symbol, to});
                trie.emplace_back();
            }
            state = to;
        }
        long long& multiplicity =
            trie[static_cast<std::size_t>(state)].multiplicity;
        if(multiplicity == std::numeric_limits<long long>::max())[[unlikely]]{
            throw std::overflow_error(
                "count_encoded_dictionary_segmentations "
                "token multiplicity overflow"
            );
        }
        ++multiplicity;
        maximum_token_length = std::max(maximum_token_length, token.size());
    }

    std::vector<long long> ways(encoded_text.size() + 1);
    ways[0] = count_limit == 0 ? 0 : 1;
    for(std::size_t start = 0; start < encoded_text.size(); ++start){
        if(ways[start] == 0) continue;
        int state = 0;
        const std::size_t scan_length = std::min(
            encoded_text.size() - start, maximum_token_length
        );
        for(std::size_t offset = 0; offset < scan_length; ++offset){
            state = transition(
                state,
                static_cast<unsigned char>(encoded_text[start + offset])
            );
            if(state == -1) break;

            const long long multiplicity =
                trie[static_cast<std::size_t>(state)].multiplicity;
            if(multiplicity == 0) continue;
            long long addition = count_limit;
            if(ways[start] <= count_limit / multiplicity){
                addition = ways[start] * multiplicity;
            }
            long long& destination = ways[start + offset + 1];
            if(destination >= count_limit - addition){
                destination = count_limit;
            }else{
                destination += addition;
            }
        }
    }
    return ways.back();
}
