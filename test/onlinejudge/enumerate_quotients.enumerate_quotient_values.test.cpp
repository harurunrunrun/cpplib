// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/enumerate_quotients

#include <iostream>

#include "../../src/algorithm/math/number_theory/enumerate_quotient_values.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    long long number;
    std::cin >> number;
    const auto quotients = math::enumerate_quotient_values(number);
    std::cout << quotients.size() << '\n';
    for(std::size_t index = 0; index < quotients.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << quotients[index];
    }
    std::cout << '\n';
}
