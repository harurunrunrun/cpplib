// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/bernoulli_number

#include <cstddef>
#include <iostream>

#include "../../src/algorithm/math/bernoulli_number.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t maximum_index;
    std::cin >> maximum_index;
    const auto answer = bernoulli_numbers<998244353>(maximum_index);
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
