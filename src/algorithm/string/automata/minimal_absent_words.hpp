#ifndef CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_MINIMAL_ABSENT_WORDS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_MINIMAL_ABSENT_WORDS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace minimal_absent_words_internal{

struct State{
    std::array<int, 256> next;
    int link = -1;
    int length = 0;
    int first_end = -1;

    State(){
        next.fill(-1);
    }
};

} // namespace minimal_absent_words_internal

inline std::vector<std::string> minimal_absent_words(
    std::string_view text,
    std::vector<unsigned char> alphabet
){
    using minimal_absent_words_internal::State;
    std::sort(alphabet.begin(), alphabet.end());
    alphabet.erase(std::unique(alphabet.begin(), alphabet.end()), alphabet.end());
    std::vector<State> states(1);
    int last = 0;
    for(std::size_t position = 0; position < text.size(); ++position){
        const unsigned char symbol =
            static_cast<unsigned char>(text[position]);
        const int current = static_cast<int>(states.size());
        states.emplace_back();
        states[static_cast<std::size_t>(current)].length =
            states[static_cast<std::size_t>(last)].length + 1;
        states[static_cast<std::size_t>(current)].first_end =
            static_cast<int>(position);
        int cursor = last;
        while(cursor != -1
            && states[static_cast<std::size_t>(cursor)]
                .next[static_cast<std::size_t>(symbol)] == -1){
            states[static_cast<std::size_t>(cursor)]
                .next[static_cast<std::size_t>(symbol)] = current;
            cursor = states[static_cast<std::size_t>(cursor)].link;
        }
        if(cursor == -1){
            states[static_cast<std::size_t>(current)].link = 0;
        }else{
            const int next = states[static_cast<std::size_t>(cursor)]
                .next[static_cast<std::size_t>(symbol)];
            if(states[static_cast<std::size_t>(cursor)].length + 1
                == states[static_cast<std::size_t>(next)].length){
                states[static_cast<std::size_t>(current)].link = next;
            }else{
                const int clone = static_cast<int>(states.size());
                states.push_back(states[static_cast<std::size_t>(next)]);
                states[static_cast<std::size_t>(clone)].length =
                    states[static_cast<std::size_t>(cursor)].length + 1;
                while(cursor != -1
                    && states[static_cast<std::size_t>(cursor)]
                        .next[static_cast<std::size_t>(symbol)] == next){
                    states[static_cast<std::size_t>(cursor)]
                        .next[static_cast<std::size_t>(symbol)] = clone;
                    cursor = states[static_cast<std::size_t>(cursor)].link;
                }
                states[static_cast<std::size_t>(next)].link = clone;
                states[static_cast<std::size_t>(current)].link = clone;
            }
        }
        last = current;
    }
    std::vector<std::string> result;
    for(unsigned char symbol: alphabet){
        if(states[0].next[static_cast<std::size_t>(symbol)] == -1){
            result.emplace_back(1, static_cast<char>(symbol));
        }
    }
    for(std::size_t id = 1; id < states.size(); ++id){
        const State& state = states[id];
        const State& suffix =
            states[static_cast<std::size_t>(state.link)];
        const int representative_length = suffix.length + 1;
        const int begin = state.first_end - representative_length + 1;
        const std::string representative(
            text.substr(
                static_cast<std::size_t>(begin),
                static_cast<std::size_t>(representative_length)
            )
        );
        for(unsigned char symbol: alphabet){
            if(state.next[static_cast<std::size_t>(symbol)] == -1
                && suffix.next[static_cast<std::size_t>(symbol)] != -1){
                std::string word = representative;
                word.push_back(static_cast<char>(symbol));
                result.push_back(std::move(word));
            }
        }
    }
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

inline std::vector<std::string> minimal_absent_words(
    std::string_view text,
    std::string_view alphabet
){
    std::vector<unsigned char> symbols;
    symbols.reserve(alphabet.size());
    for(unsigned char symbol: alphabet) symbols.push_back(symbol);
    return minimal_absent_words(text, std::move(symbols));
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_MINIMAL_ABSENT_WORDS_HPP_INCLUDED
