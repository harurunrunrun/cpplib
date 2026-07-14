// competitive-verifier: PROBLEM https://www.spoj.com/problems/LCS/

#include <algorithm>
#include <iostream>
#include <string>

#include "../../src/algorithm/string/suffix_automaton.hpp"

namespace{
SuffixAutomaton<26, 500001> automaton;
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string first;
    std::string second;
    if(!(std::cin >> first >> second)) return 0;
    if(second.size() < first.size()) std::swap(first, second);
    for(char c: first) automaton.extend(c);
    std::cout << automaton.longest_common_substring(second).length << '\n';
}
