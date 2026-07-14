#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

namespace longest_common_substring_length_internal{

template<int CAPACITY>
class DenseSuffixAutomaton{
    struct State{
        std::array<int, CAPACITY> next{};
        int link = -1;
        int length = 0;

        State(){ next.fill(-1); }
    };

    std::vector<State> states_{1};
    int last_ = 0;

public:
    explicit DenseSuffixAutomaton(std::size_t maximum_length){
        states_.reserve(maximum_length * 2);
    }

    void extend(int symbol){
        const int current = static_cast<int>(states_.size());
        states_.emplace_back();
        states_[current].length = states_[last_].length + 1;

        int state = last_;
        while(state != -1 && states_[state].next[symbol] == -1){
            states_[state].next[symbol] = current;
            state = states_[state].link;
        }
        if(state == -1){
            states_[current].link = 0;
        }else{
            const int next = states_[state].next[symbol];
            if(states_[state].length + 1 == states_[next].length){
                states_[current].link = next;
            }else{
                const int clone = static_cast<int>(states_.size());
                states_.push_back(states_[next]);
                states_[clone].length = states_[state].length + 1;
                while(state != -1 && states_[state].next[symbol] == next){
                    states_[state].next[symbol] = clone;
                    state = states_[state].link;
                }
                states_[next].link = states_[current].link = clone;
            }
        }
        last_ = current;
    }

    template<class SymbolOf>
    int match(std::string_view text, SymbolOf symbol_of) const{
        int state = 0;
        int length = 0;
        int answer = 0;
        for(unsigned char byte: text){
            const int symbol = symbol_of(byte);
            if(symbol < 0){
                state = 0;
                length = 0;
                continue;
            }
            while(state != 0 && states_[state].next[symbol] == -1){
                state = states_[state].link;
                length = states_[state].length;
            }
            const int next = states_[state].next[symbol];
            if(next == -1){
                state = 0;
                length = 0;
            }else{
                state = next;
                length++;
                answer = std::max(answer, length);
            }
        }
        return answer;
    }
};

class SparseSuffixAutomaton{
    struct State{
        std::vector<std::pair<unsigned char, int>> next;
        int link = -1;
        int length = 0;
    };

    std::vector<State> states_{1};
    int last_ = 0;

    int transition(int state, unsigned char byte) const{
        for(const auto [label, destination]: states_[state].next){
            if(label == byte) return destination;
        }
        return -1;
    }

    void set_transition(int state, unsigned char byte, int destination){
        for(auto& [label, current]: states_[state].next){
            if(label == byte){
                current = destination;
                return;
            }
        }
        states_[state].next.emplace_back(byte, destination);
    }

public:
    explicit SparseSuffixAutomaton(std::size_t maximum_length){
        states_.reserve(maximum_length * 2);
    }

    void extend(unsigned char byte){
        const int current = static_cast<int>(states_.size());
        states_.emplace_back();
        states_[current].length = states_[last_].length + 1;

        int state = last_;
        while(state != -1 && transition(state, byte) == -1){
            set_transition(state, byte, current);
            state = states_[state].link;
        }
        if(state == -1){
            states_[current].link = 0;
        }else{
            const int next = transition(state, byte);
            if(states_[state].length + 1 == states_[next].length){
                states_[current].link = next;
            }else{
                const int clone = static_cast<int>(states_.size());
                states_.push_back(states_[next]);
                states_[clone].length = states_[state].length + 1;
                while(state != -1 && transition(state, byte) == next){
                    set_transition(state, byte, clone);
                    state = states_[state].link;
                }
                states_[next].link = states_[current].link = clone;
            }
        }
        last_ = current;
    }

    int match(std::string_view text) const{
        int state = 0;
        int length = 0;
        int answer = 0;
        for(unsigned char byte: text){
            while(state != 0 && transition(state, byte) == -1){
                state = states_[state].link;
                length = states_[state].length;
            }
            const int next = transition(state, byte);
            if(next == -1){
                state = 0;
                length = 0;
            }else{
                state = next;
                length++;
                answer = std::max(answer, length);
            }
        }
        return answer;
    }
};

}  // namespace longest_common_substring_length_internal

inline int longest_common_substring_length(
    std::string_view first,
    std::string_view second
){
    if(first.size() > second.size()) std::swap(first, second);
    if(first.empty()) return 0;
    if(first.size() > static_cast<std::size_t>((std::numeric_limits<int>::max() - 1) / 2)){
        throw std::length_error("longest_common_substring_length: input is too long");
    }

    std::array<int, 256> symbol{};
    symbol.fill(-1);
    int alphabet_size = 0;
    for(unsigned char byte: first){
        if(symbol[byte] == -1) symbol[byte] = alphabet_size++;
    }

    if(alphabet_size <= 4){
        longest_common_substring_length_internal::DenseSuffixAutomaton<4> automaton(first.size());
        for(unsigned char byte: first) automaton.extend(symbol[byte]);
        return automaton.match(second, [&](unsigned char byte){ return symbol[byte]; });
    }
    if(alphabet_size <= 8){
        longest_common_substring_length_internal::DenseSuffixAutomaton<8> automaton(first.size());
        for(unsigned char byte: first) automaton.extend(symbol[byte]);
        return automaton.match(second, [&](unsigned char byte){ return symbol[byte]; });
    }

    longest_common_substring_length_internal::SparseSuffixAutomaton automaton(first.size());
    for(unsigned char byte: first) automaton.extend(byte);
    return automaton.match(second);
}
