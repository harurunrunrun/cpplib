// competitive-verifier: PROBLEM https://www.spoj.com/problems/NSUBSTR/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/suffix_automaton.hpp"

namespace{
SuffixAutomaton<26, 500001> automaton;
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    if(!(std::cin >> text)) return 0;
    for(char c: text) automaton.extend(c);
    for(long long count: automaton.maximum_occurrences_by_length()){
        std::cout << count << '\n';
    }
}
