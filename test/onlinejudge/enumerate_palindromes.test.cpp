// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/enumerate_palindromes

#include <iostream>
#include <string>
#include "../../src/algorithm/string/manacher.hpp"

int main(){
    std::string text;
    std::cin >> text;
    const auto odd = manacher_odd(text);
    const auto even = manacher_even(text);
    for(int center = 0; center + 1 < 2 * static_cast<int>(text.size()); ++center){
        if(center != 0) std::cout << ' ';
        if((center & 1) == 0){
            std::cout << 2 * odd[static_cast<std::size_t>(center / 2)] - 1;
        }else{
            std::cout << 2 * even[static_cast<std::size_t>(center / 2 + 1)];
        }
    }
    std::cout << '\n';
}
