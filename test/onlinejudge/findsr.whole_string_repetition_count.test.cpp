// competitive-verifier: PROBLEM https://www.spoj.com/problems/FINDSR/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/whole_string_repetition_count.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    while(std::cin >> text && text != "*"){
        std::cout << whole_string_repetition_count(text) << '\n';
    }
}
