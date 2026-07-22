// competitive-verifier: PROBLEM https://www.spoj.com/problems/EPALIN/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/palindrome/shortest_palindrome_by_appending.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    while(std::getline(std::cin, text)){
        if(!text.empty() && text.back() == '\r') text.pop_back();
        std::cout << shortest_palindrome_by_appending(text) << '\n';
    }
}
