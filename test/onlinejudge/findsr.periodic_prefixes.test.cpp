// competitive-verifier: PROBLEM https://www.spoj.com/problems/FINDSR/

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/string/periodic_prefixes.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    while(std::cin >> text && text != "*"){
        const std::vector<std::pair<int, int>> prefixes = periodic_prefixes(text);
        const int exponent = !prefixes.empty() && prefixes.back().first ==
            static_cast<int>(text.size())
            ? prefixes.back().second
            : 1;
        std::cout << exponent << '\n';
    }
}
