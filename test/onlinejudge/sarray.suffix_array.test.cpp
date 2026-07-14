// competitive-verifier: PROBLEM https://www.spoj.com/problems/SARRAY/

#include <iostream>
#include <string>

#include "../../src/algorithm/string/suffix_array.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    if(!(std::cin >> text)) return 0;
    for(const int position: suffix_array(text)){
        std::cout << position << '\n';
    }
}
