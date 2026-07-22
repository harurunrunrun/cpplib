// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/montmort_number_mod

#include <cstddef>
#include <cstdint>
#include <iostream>

#include "../../src/algorithm/math/combinatorics/montmort_number.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t maximum_size;
    std::uint32_t modulus;
    std::cin >> maximum_size >> modulus;
    const auto answer = montmort_numbers(maximum_size, modulus);
    for(std::size_t size = 1; size <= maximum_size; ++size){
        if(size != 1) std::cout << ' ';
        std::cout << answer[size];
    }
    std::cout << '\n';
}
