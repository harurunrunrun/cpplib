// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/stirling_number_of_the_first_kind

#include <cstddef>
#include <iostream>

#include "../../src/algorithm/math/combinatorics/stirling_numbers.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t order;
    std::cin >> order;
    const auto answer =
        math::signed_stirling_numbers_first_kind<998244353>(order);
    for(std::size_t index = 0; index < answer.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << answer[index];
    }
    std::cout << '\n';
}
