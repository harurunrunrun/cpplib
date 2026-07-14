#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

inline long long count_morse_decodings(
    const std::string& signal,
    const std::vector<std::string>& words,
    long long count_limit = std::numeric_limits<long long>::max()
){
    if(count_limit < 0)[[unlikely]]{
        throw std::invalid_argument("count_morse_decodings negative limit");
    }
    constexpr std::array<std::string_view, 26> MORSE = {
        ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
        ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
        "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..",
    };
    for(const char symbol: signal){
        if(symbol != '.' && symbol != '-')[[unlikely]]{
            throw std::invalid_argument("count_morse_decodings invalid signal");
        }
    }
    struct Node{
        std::array<int, 2> next = {-1, -1};
        int multiplicity = 0;
    };
    std::vector<Node> trie(1);
    std::size_t maximum_length = 0;
    for(const auto& word: words){
        if(word.empty())[[unlikely]]{
            throw std::invalid_argument("count_morse_decodings empty word");
        }
        int state = 0;
        std::size_t encoded_length = 0;
        for(const char character: word){
            if(character < 'A' || 'Z' < character)[[unlikely]]{
                throw std::invalid_argument(
                    "count_morse_decodings invalid word character"
                );
            }
            for(const char symbol: MORSE[static_cast<std::size_t>(
                character - 'A'
            )]){
                const int letter = symbol == '-' ? 1 : 0;
                int& to = trie[static_cast<std::size_t>(state)]
                    .next[static_cast<std::size_t>(letter)];
                if(to == -1){
                    if(trie.size() >= static_cast<std::size_t>(
                        std::numeric_limits<int>::max()
                    ))[[unlikely]]{
                        throw std::length_error(
                            "count_morse_decodings dictionary too large"
                        );
                    }
                    const int created = static_cast<int>(trie.size());
                    to = created;
                    trie.emplace_back();
                    state = created;
                }else{
                    state = to;
                }
                ++encoded_length;
            }
        }
        int& multiplicity = trie[static_cast<std::size_t>(state)].multiplicity;
        if(multiplicity == std::numeric_limits<int>::max())[[unlikely]]{
            throw std::overflow_error(
                "count_morse_decodings word multiplicity overflow"
            );
        }
        ++multiplicity;
        if(maximum_length < encoded_length) maximum_length = encoded_length;
    }

    std::vector<long long> ways(signal.size() + 1);
    ways[0] = count_limit == 0 ? 0 : 1;
    for(std::size_t start = 0; start < signal.size(); ++start){
        if(ways[start] == 0) continue;
        int state = 0;
        const std::size_t available = signal.size() - start;
        const std::size_t length = std::min(available, maximum_length);
        for(std::size_t offset = 0; offset < length; ++offset){
            const int letter = signal[start + offset] == '-' ? 1 : 0;
            state = trie[static_cast<std::size_t>(state)]
                .next[static_cast<std::size_t>(letter)];
            if(state == -1) break;
            const int multiplicity =
                trie[static_cast<std::size_t>(state)].multiplicity;
            if(multiplicity == 0) continue;
            long long addition = count_limit;
            if(ways[start] <= count_limit / multiplicity){
                addition = ways[start] * multiplicity;
            }
            long long& destination = ways[start + offset + 1];
            if(addition < count_limit - destination){
                destination += addition;
            }else{
                destination = count_limit;
            }
        }
    }
    return ways.back();
}
