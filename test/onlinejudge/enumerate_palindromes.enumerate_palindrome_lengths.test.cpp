// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/enumerate_palindromes

#include <iostream>
#include <string>
#include "../../src/algorithm/string/enumerate_palindrome_lengths.hpp"

int main(){
    std::string text;
    std::cin >> text;
    const auto answer = enumerate_palindrome_lengths(text);
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
