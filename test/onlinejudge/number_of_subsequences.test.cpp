// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/number_of_subsequences

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/combinatorics/number_of_subsequences.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t size;
    std::cin >> size;
    std::vector<std::uint32_t> values(size);
    for(std::uint32_t& value: values) std::cin >> value;
    std::cout << number_of_distinct_subsequences(values, 998244353U) << '\n';
}
