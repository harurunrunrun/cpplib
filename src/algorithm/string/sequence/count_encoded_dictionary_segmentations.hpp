#ifndef CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_COUNT_ENCODED_DICTIONARY_SEGMENTATIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_COUNT_ENCODED_DICTIONARY_SEGMENTATIONS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <queue>
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
        std::vector<std::pair<unsigned char, int>> automaton;
        long long multiplicity = 0;
        std::size_t depth = 0;
        int failure = 0;
        int output_link = -1;
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
                trie[static_cast<std::size_t>(to)].depth =
                    trie[static_cast<std::size_t>(state)].depth + 1;
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
    }

    for(Node& node: trie) node.automaton = node.next;
    const auto automaton_transition = [&](int state, unsigned char symbol){
        std::vector<int> uncached;
        int current = state;
        int target = 0;
        while(true){
            const auto& transitions =
                trie[static_cast<std::size_t>(current)].automaton;
            const auto position = std::lower_bound(
                transitions.begin(), transitions.end(), symbol,
                [](const auto& edge, unsigned char value){
                    return edge.first < value;
                }
            );
            if(position != transitions.end() && position->first == symbol){
                target = position->second;
                break;
            }
            uncached.push_back(current);
            if(current == 0) break;
            current = trie[static_cast<std::size_t>(current)].failure;
        }
        for(const int missing_state: uncached){
            auto& transitions =
                trie[static_cast<std::size_t>(missing_state)].automaton;
            const auto position = std::lower_bound(
                transitions.begin(), transitions.end(), symbol,
                [](const auto& edge, unsigned char value){
                    return edge.first < value;
                }
            );
            transitions.insert(position, {symbol, target});
        }
        return target;
    };

    std::queue<int> queue;
    for(const auto& [symbol, child]: trie[0].next){
        static_cast<void>(symbol);
        trie[static_cast<std::size_t>(child)].failure = 0;
        trie[static_cast<std::size_t>(child)].output_link = -1;
        queue.push(child);
    }
    while(!queue.empty()){
        const int state = queue.front();
        queue.pop();
        for(const auto& [symbol, child]:
            trie[static_cast<std::size_t>(state)].next){
            const int target = automaton_transition(
                trie[static_cast<std::size_t>(state)].failure,
                symbol
            );
            Node& child_node = trie[static_cast<std::size_t>(child)];
            child_node.failure = target;
            child_node.output_link =
                trie[static_cast<std::size_t>(target)].multiplicity != 0
                ? target
                : trie[static_cast<std::size_t>(target)].output_link;
            queue.push(child);
        }
    }

    std::vector<long long> ways(encoded_text.size() + 1);
    ways[0] = count_limit == 0 ? 0 : 1;
    int state = 0;
    for(std::size_t end = 1; end <= encoded_text.size(); ++end){
        const unsigned char symbol =
            static_cast<unsigned char>(encoded_text[end - 1]);
        state = automaton_transition(state, symbol);
        for(int output =
                trie[static_cast<std::size_t>(state)].multiplicity != 0
                ? state
                : trie[static_cast<std::size_t>(state)].output_link;
            output != -1;
            output = trie[static_cast<std::size_t>(output)].output_link){
            const Node& token = trie[static_cast<std::size_t>(output)];
            const std::size_t start = end - token.depth;
            if(ways[start] == 0) continue;
            long long addition = count_limit;
            if(ways[start] <= count_limit / token.multiplicity){
                addition = ways[start] * token.multiplicity;
            }
            long long& destination = ways[end];
            if(destination >= count_limit - addition){
                destination = count_limit;
            }else{
                destination += addition;
            }
        }
    }
    return ways.back();
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_COUNT_ENCODED_DICTIONARY_SEGMENTATIONS_HPP_INCLUDED
