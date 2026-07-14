// competitive-verifier: PROBLEM https://www.spoj.com/problems/ADAPHOTO/

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/string/suffix_automaton.hpp"

namespace{

SuffixAutomaton<4, 2000000> automaton;

char encode_character(char character){
    switch(character){
        case '^': return 'a';
        case 'v': return 'b';
        case '-': return 'c';
        case '~': return 'd';
        default: throw std::runtime_error("invalid ADAPHOTO character");
    }
}

void encode(std::string& text){
    for(char& character: text) character = encode_character(character);
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string first, second;
    if(!(std::cin >> first >> second)) return 0;
    if(second.size() < first.size()) std::swap(first, second);
    encode(first);
    encode(second);
    for(char character: first) automaton.extend(character);
    std::cout << automaton.longest_common_substring(second).length << '\n';
}
