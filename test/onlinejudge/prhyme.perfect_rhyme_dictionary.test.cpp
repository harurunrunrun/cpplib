// competitive-verifier: PROBLEM https://www.spoj.com/problems/PRHYME/

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/string/suffix/longest_common_suffix_dictionary.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<std::string> dictionary_words;
    std::string line;
    while(std::getline(std::cin, line)){
        if(!line.empty() && line.back() == '\r') line.pop_back();
        if(line.empty()) break;
        dictionary_words.push_back(line);
    }

    LongestCommonSuffixDictionary dictionary(std::move(dictionary_words));
    while(std::getline(std::cin, line)){
        if(!line.empty() && line.back() == '\r') line.pop_back();
        if(line.empty()) continue;
        std::cout << dictionary.best_match(line).value() << '\n';
    }
}
