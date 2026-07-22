// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/zalgorithm

#include <iostream>
#include <string>
#include "../../src/algorithm/string/automata/z_algorithm.hpp"

int main(){
    std::string text;
    std::cin >> text;
    const auto z = z_algorithm(text);
    for(std::size_t i = 0; i < z.size(); ++i){
        if(i != 0) std::cout << ' ';
        std::cout << z[i];
    }
    std::cout << '\n';
}
