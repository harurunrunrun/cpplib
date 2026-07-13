// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/lyndon_factorization

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/lyndon_factorization.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    std::cin >> text;
    const std::vector<std::size_t> boundaries = lyndon_factorization(text);
    for(std::size_t index = 0; index < boundaries.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << boundaries[index];
    }
    std::cout << '\n';
}
