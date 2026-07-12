// competitive-verifier: STANDALONE

#include "../../src/algorithm/string/suffix_automaton.hpp"

#include <iostream>
#include <string>

int main(){
    int case_count;
    if(!(std::cin >> case_count)) return 0;

    for(int case_index = 0; case_index < case_count; case_index++){
        std::string source, target;
        int query_count;
        std::cin >> source >> target >> query_count;

        SuffixAutomaton<4, 256> automaton;
        for(char c: source){
            automaton.extend(c);
            automaton.build_occurrences();
        }

        auto match = automaton.longest_common_substring(target);
        if(match.position < 0 ||
           match.position + match.length > static_cast<int>(target.size()) ||
           !automaton.contains(target.substr(
               static_cast<std::size_t>(match.position),
               static_cast<std::size_t>(match.length)
           ))){
            return 1;
        }

        std::cout << automaton.count_distinct_substrings()
                  << ' ' << match.length << '\n';
        for(int query_index = 0; query_index < query_count; query_index++){
            std::string pattern;
            std::cin >> pattern;
            std::cout << automaton.contains(pattern)
                      << ' ' << automaton.occurrence_count(pattern) << '\n';
        }
    }
}