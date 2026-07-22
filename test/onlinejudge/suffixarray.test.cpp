// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/suffixarray

#include <iostream>
#include <string>
#include "../../src/algorithm/string/suffix/suffix_array.hpp"

int main(){
    std::string text;
    std::cin >> text;
    const auto answer = suffix_array(text);
    for(std::size_t i = 0; i < answer.size(); ++i){
        if(i != 0) std::cout << ' ';
        std::cout << answer[i];
    }
    std::cout << '\n';
}
