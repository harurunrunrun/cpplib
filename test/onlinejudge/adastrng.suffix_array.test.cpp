// competitive-verifier: PROBLEM https://www.spoj.com/problems/ADASTRNG/

#include <array>
#include <iostream>
#include <string>

#include "../../src/algorithm/string/suffix_array.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    if(!(std::cin >> text)) return 0;
    const std::array<long long, 256> count =
        number_of_distinct_substrings_by_first_byte(text);
    for(int letter = 0; letter < 26; letter++){
        if(letter != 0) std::cout << ' ';
        std::cout << count[static_cast<unsigned char>('a' + letter)];
    }
    std::cout << '\n';
}
