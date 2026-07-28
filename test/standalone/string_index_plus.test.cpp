// competitive-verifier: STANDALONE

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/automata/dynamic_rolling_hash.hpp"
#include "../../src/algorithm/string/automata/prefix_automaton.hpp"
#include "../../src/algorithm/string/automata/shift_and.hpp"
#include "../../src/algorithm/string/automata/shift_or.hpp"
#include "../../src/algorithm/string/suffix/burrows_wheeler_transform.hpp"
#include "../../src/algorithm/string/suffix/fm_index.hpp"
#include "../../src/algorithm/string/suffix/longest_common_extension.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int kind;
        std::cin >> kind;
        if(kind == 0){
            std::string text;
            std::cin >> text;
            auto result = burrows_wheeler_transform(text);
            if(inverse_burrows_wheeler_transform(
                result.transformed, result.primary_index
            ) != text) return 2;
            std::cout << result.primary_index << ' '
                      << result.transformed << '\n';
        }else if(kind == 1){
            std::string text;
            int query_count;
            std::cin >> text >> query_count;
            FMIndex index(text);
            for(int query = 0; query < query_count; ++query){
                std::string pattern;
                std::cin >> pattern;
                auto positions = index.locate(pattern);
                std::sort(positions.begin(), positions.end());
                if(static_cast<int>(positions.size())
                    != index.count(pattern)) return 3;
                std::cout << positions.size();
                for(int position: positions) std::cout << ' ' << position;
                std::cout << '\n';
            }
        }else if(kind == 2){
            std::string text;
            std::string pattern;
            std::cin >> text >> pattern;
            const auto shift_and = shift_and_match(text, pattern);
            const auto shift_or = shift_or_match(text, pattern);
            const PrefixAutomaton automaton(pattern);
            const auto prefix = automaton.match_positions(text);
            if(shift_and != shift_or || shift_and != prefix) return 4;
            std::cout << shift_and.size();
            for(int position: shift_and) std::cout << ' ' << position;
            std::cout << '\n';
        }else if(kind == 3){
            std::string text;
            int query_count;
            std::cin >> text >> query_count;
            LongestCommonExtension lce(text);
            for(int query = 0; query < query_count; ++query){
                int first;
                int second;
                std::cin >> first >> second;
                std::cout << lce.lce(first, second) << '\n';
            }
        }else{
            std::string text;
            int query_count;
            std::cin >> text >> query_count;
            DynamicRollingHash hash(text);
            for(int query = 0; query < query_count; ++query){
                int operation;
                std::cin >> operation;
                if(operation == 0){
                    int position;
                    char symbol;
                    std::cin >> position >> symbol;
                    text[static_cast<std::size_t>(position)] = symbol;
                    hash.set(position, static_cast<unsigned char>(symbol));
                }else{
                    int first;
                    int second;
                    std::cin >> first >> second;
                    const int result = hash.lcp(first, second);
                    int exact = 0;
                    while(first + exact < static_cast<int>(text.size())
                        && second + exact < static_cast<int>(text.size())
                        && text[static_cast<std::size_t>(first + exact)]
                            == text[static_cast<std::size_t>(second + exact)]){
                        ++exact;
                    }
                    if(result != exact) return 5;
                    std::cout << result << '\n';
                }
            }
        }
    }
}
