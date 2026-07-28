#ifndef CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_PREFIX_AUTOMATON_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_PREFIX_AUTOMATON_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

#include "prefix_function.hpp"

struct PrefixAutomaton{
    std::string pattern;
    std::vector<std::array<int, 256>> transition;

    explicit PrefixAutomaton(std::string pattern_)
        : pattern(std::move(pattern_)),
          transition(pattern.size() + 1){
        const std::vector<int> prefix = prefix_function(pattern);
        for(std::size_t state = 0; state <= pattern.size(); ++state){
            for(int symbol = 0; symbol < 256; ++symbol){
                if(state < pattern.size()
                    && static_cast<unsigned char>(pattern[state]) == symbol){
                    transition[state][static_cast<std::size_t>(symbol)] =
                        static_cast<int>(state + 1);
                }else if(state == 0){
                    transition[state][static_cast<std::size_t>(symbol)] = 0;
                }else{
                    transition[state][static_cast<std::size_t>(symbol)] =
                        transition[static_cast<std::size_t>(
                            prefix[state - 1]
                        )][static_cast<std::size_t>(symbol)];
                }
            }
        }
    }

    int next_state(int state, unsigned char symbol) const{
        if(state < 0 || static_cast<int>(transition.size()) <= state)
            [[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(PrefixAutomaton::next_state)."
            );
        }
        return transition[static_cast<std::size_t>(state)]
            [static_cast<std::size_t>(symbol)];
    }

    std::vector<int> match_positions(std::string_view text) const{
        std::vector<int> result;
        if(pattern.empty()){
            result.resize(text.size() + 1);
            for(std::size_t index = 0; index <= text.size(); ++index){
                result[index] = static_cast<int>(index);
            }
            return result;
        }
        int state = 0;
        for(std::size_t index = 0; index < text.size(); ++index){
            state = next_state(
                state, static_cast<unsigned char>(text[index])
            );
            if(state == static_cast<int>(pattern.size())){
                result.push_back(
                    static_cast<int>(index + 1 - pattern.size())
                );
            }
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_PREFIX_AUTOMATON_HPP_INCLUDED
