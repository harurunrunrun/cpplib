// competitive-verifier: PROBLEM https://www.spoj.com/problems/LCS2/

#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "../../src/algorithm/string/suffix_automaton.hpp"

namespace{
SuffixAutomaton<26, 200001> automaton;
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<std::string> texts;
    for(std::string text; std::cin >> text;) texts.push_back(std::move(text));
    if(texts.empty()) return 0;

    const auto shortest = std::min_element(
        texts.begin(), texts.end(),
        [](const std::string& lhs, const std::string& rhs){
            return lhs.size() < rhs.size();
        }
    );
    std::swap(texts.front(), *shortest);
    for(char c: texts.front()) automaton.extend(c);

    std::vector<std::string_view> additional;
    additional.reserve(texts.size() - 1);
    for(std::size_t index = 1; index < texts.size(); index++){
        additional.push_back(texts[index]);
    }
    std::cout << automaton.longest_common_substring_length(additional) << '\n';
}
