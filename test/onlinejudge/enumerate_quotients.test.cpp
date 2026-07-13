// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/enumerate_quotients

#include <algorithm>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/basic_number_theory.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    long long number;
    std::cin >> number;
    const auto ranges = math::quotient_ranges(number);
    std::vector<long long> quotients;
    quotients.reserve(ranges.size());
    for(const auto& range: ranges) quotients.push_back(range.value);
    std::reverse(quotients.begin(), quotients.end());
    std::cout << quotients.size() << '\n';
    for(std::size_t index = 0; index < quotients.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << quotients[index];
    }
    std::cout << '\n';
}
