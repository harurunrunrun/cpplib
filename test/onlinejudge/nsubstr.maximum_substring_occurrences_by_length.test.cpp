// competitive-verifier: PROBLEM https://www.spoj.com/problems/NSUBSTR/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/maximum_substring_occurrences_by_length.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    if(!(std::cin >> text)) return 0;
    for(const long long count:
        maximum_substring_occurrences_by_length<250000>(text)){
        std::cout << count << '\n';
    }
}
